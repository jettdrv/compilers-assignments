/**
  * @file LoopFusion.cpp
  * @brief Implementazione del pass per la Loop Fusion
 */

#include "LoopFusion.h"

using namespace llvm;

namespace {
//FUNZIONI DI SERVIZIO 

/// @brief Funzione che controlla se un loop è adatto alla fusione.
/// @details Per essere fuso, un loop deve essere in forma normale, deve possedere tutti i blocchi importanti come il preheader, header, latch ed exiting blocks. Inoltre non deve lanciare eccezioni e non deve contenere accessi volatili sulla memoria.
/// @param L Loop da controllare
/// @return true se il loop L è candidato per la fusione, false altrimenti

static bool isFusionCandidate(Loop *L){

    if(!L->isLoopSimplifyForm())
      return false;

    if(!L->getHeader() || !L->getLoopPreheader() || !L->getLoopLatch() || !L->getExitingBlock() || !L->getUniqueExitBlock())
      return false;

    //check dei requisiti sulle istruzioni
    for(BasicBlock *BB : L->blocks()){
      for(Instruction &I : *BB){
        //se vero, l'istruzione può lanciare un eccezione
        if(I.mayThrow())
          return false;
        //vero se contiene acessi volatili sulla memoria
        if(I.isVolatile())
          return false;
      }
    }
    return true;
}


/// @brief Procedura che ordina i loop per dominanza
/// @param Set il set di loop che sono Contol Flow Equivalenti
/// @param DT Dominator tree

static void sortCFESet(SmallVector<Loop *, 4> &Set, DominatorTree &DT){

  for (size_t i = 0; i < Set.size(); ++i) {
    for (size_t j = i + 1; j < Set.size(); ++j) {
      if (DT.dominates(Set[j]->getHeader(), Set[i]->getHeader())) {
        std::swap(Set[i], Set[j]);
      }
    }
  }
}

/// @brief Funzione che verifica l'adiacenza tra due loop.
/// @details Se i loop sono guarded, il successore non loop del guard branch del primo loop deve essere l'entry block dell'altro. Questo si verifica ottenendo i blocchi successori del guard branch e controllando se fanno parte del loop. Appena troviamo il successore che non appartiene il loop, verifichiamo se è uguale al preheader del secondo loop. Se i loop sono unguarded l'exit block del primo loop deve essere il preheader del secondo loop.
/// @param L1
/// @param L2
/// @return true se sono adiacenti, false altrimenti

static bool areAdjacent(Loop *L0, Loop *L1){
  BasicBlock *preheader1 = L1->getLoopPreheader();
  if(L0->isGuarded() && L1->isGuarded()){
    BranchInst *guardInst = L0->getLoopGuardBranch();

    if(!guardInst || guardInst->getNumSuccessors() != 2)
      return false;
    //la guard instruction ha due successori e il secondo loop potrebbe essere in uno dei due successori. se uno dei successori del guard instruction non è contenuto nel loop, allora tale successore deve essere il preheader del secondo loop
    for(int i = 0; i<2; i++){
      BasicBlock *succ = guardInst->getSuccessor(i);
      if(!L0->contains(succ)){
        return succ == preheader1;
      }
    }
  }else{
    if(L0->getUniqueExitBlock() == preheader1)
      return true;
  }
  return false;
}

/// @brief Funzione che confronta il loop trip count di due loop utilizzando la Scalar Evolution
/// @param L0
/// @param L1
/// @param SE
/// @return true se hanno loop trip count identico, false altrimenti

static bool haveIdloopTripCount(Loop *L0, Loop *L1, ScalarEvolution &SE ){

  //quante volte viene seguito il backedge del loop
  const SCEV *backedgecount0 = SE.getBackedgeTakenCount(L0);
  const SCEV *backedgecount1 = SE.getBackedgeTakenCount(L1);

  //se la scalar evolution non è riuscita a definire nessun valore di loop trip anche solo per uno dei loop, non si riesce a fare il confronto
  if (isa<SCEVCouldNotCompute>(backedgecount0) || isa<SCEVCouldNotCompute>(backedgecount1)) {
        return false; 
    }
    return backedgecount0 == backedgecount1;
}

/// @brief Funzione che determina se due loop sono control flow equivalenti, ovvero se L0 domina L1, allora L1 postdomina L0
/// @param L0
/// @param L1
/// @param DT
/// @param PDT
/// @return true se L0 e L1 sono CFE, false altrimenti

static bool areControlFlowEq(Loop *L0, Loop *L1, DominatorTree &DT, PostDominatorTree &PDT){
  BasicBlock *header0 = L0->getHeader();
  BasicBlock *header1 = L1->getHeader();

  return DT.dominates(header0, header1) && PDT.dominates(header1, header0);

}

/// @brief Funzione che analizza le dipendenze
/// @param L0
/// @param L1
/// @param DI
/// @return true se le dipendenze sono valide, false altrimenti

static bool dependenceAnalysis(Loop *L0, Loop *L1, DependenceInfo &DI){

  SmallVector <Instruction *, 8> loadStoreIns0;
  SmallVector <Instruction *, 8> loadStoreIns1;


    for(BasicBlock *BB: L0->blocks())
      for(Instruction &I: *BB)
        if(I.mayReadOrWriteMemory())
          loadStoreIns0.push_back(&I);

    for(BasicBlock *BB: L1->blocks())
      for(Instruction &I: *BB)
        if(I.mayReadOrWriteMemory())
          loadStoreIns1.push_back(&I);

    for(Instruction *I0: loadStoreIns0){
      for(Instruction *I1: loadStoreIns1){

        std::unique_ptr <Dependence> Dep = DI.depends(I0, I1,true);
          if(Dep)
            return false;
      }
    }

  return true;
}

/// @brief Funzione che trova il primo blocco del corpo di un loop
/// @param L
/// @return Succ se trova il successore, nullptr altrimenti

static BasicBlock *getLoopBodyEntry(Loop *L) {
    BasicBlock *ExitingBB = L->getExitingBlock();
    for (BasicBlock *Succ : successors(ExitingBB)) {
        if (L->contains(Succ))
            return Succ;
    }
    return nullptr;
}


/// @brief Funzione che fonde i due loop e ritorna il loop fuso 
/// @param L0
/// @param L1
/// @param LI
/// @return L0 fuso

static Loop* fuseLoops(Loop *L0, Loop *L1, LoopInfo &LI){
  //recupero i blocchi principali
  BasicBlock *header0 = L0->getHeader();
  BasicBlock *latch0 = L0->getLoopLatch();
  BasicBlock *body0 = getLoopBodyEntry(L0);
  BasicBlock *exitingBlock0 = L0->getExitingBlock();

  BasicBlock *header1 = L1->getHeader();
  BasicBlock *preheader1 = L1->getLoopPreheader();
  BasicBlock *latch1 = L1->getLoopLatch();
  BasicBlock *exit1 = L1->getUniqueExitBlock();
  BasicBlock *body1 = getLoopBodyEntry(L1);


  //1)modifica della induction variable 
  PHINode *IV0 = L0->getCanonicalInductionVariable();
  PHINode *IV1 = L1->getCanonicalInductionVariable();

  if(IV0 && IV1){
    IV1->replaceAllUsesWith(IV0);
    IV1->eraseFromParent();
  }
  

  //2) riscrittura del CFG
  exitingBlock0->getTerminator()->replaceSuccessorWith(preheader1, exit1);
  body0->getTerminator()->replaceSuccessorWith(latch0, body1);
  body1->getTerminator()->replaceSuccessorWith(latch1, latch0);
  
  SmallVector<BasicBlock *, 8> L1Blocks(L1->blocks().begin(), L1->blocks().end());
  for (BasicBlock *BB : L1Blocks)
      if (BB != header1 && BB != preheader1 && BB != latch1){
        LI.removeBlock(BB);
        L0->addBlockEntry(BB);
        LI.changeLoopFor(BB, L0);
      }
       
      LI.erase(L1);

  DeleteDeadBlock(header1);
  DeleteDeadBlock(preheader1);
  DeleteDeadBlock(latch1);

    return L0;

}
}


//-------------------------------------------------------------------------------


/// @brief Function pass che effettua tutte le trasformazioni sui loop
/// @param F Funzione su cui applicare i passi di trasformazione
/// @param AM New pass manager per funzioni
/// @return Un oggetto PreservedAnalyses che indica quali analisi sono rimaste valide dopo il pass

PreservedAnalyses LoopFusionPass::run(Function &F, FunctionAnalysisManager &AM) {
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
    PostDominatorTree &PDT = AM.getResult<PostDominatorTreeAnalysis>(F);
    ScalarEvolution &SE = AM.getResult<ScalarEvolutionAnalysis>(F);
    DependenceInfo &DI = AM.getResult<DependenceAnalysis>(F);


    //verifica se il CFG corrente contiene loop
    if(LI.begin() == LI.end()){
      outs()<<"Il CFG non contiene loops \n";
      return PreservedAnalyses::all();
    }

    //worklist di loop più interni
    SmallVector <Loop *, 8> Worklist;

    for (Loop *TopLevelLoop: LI){
      for (Loop *L: depth_first(TopLevelLoop)){
        if(L->isInnermost() && isFusionCandidate(L))
            Worklist.push_back(L);
      }
    }
    
    SmallVector<SmallVector<Loop *, 4>, 8> CFESets;

    while(!Worklist.empty()){
      //rimuoviamo un loop dalla worklist e troviamo gli loop che sono Control Flow Equivalent. Creiamo un insieme di questi loop.
      Loop *L = Worklist.pop_back_val();

      bool foundSet = false;
      for(SmallVector<Loop *, 4> &Set : CFESets){
        Loop *firstLoop = Set[0];
        if(areControlFlowEq(L, firstLoop, DT, PDT) || areControlFlowEq(firstLoop, L, DT, PDT)){
          Set.push_back(L);
          foundSet = true;
          break;
        }
      }
      if(!foundSet){
        SmallVector<Loop *, 4> newSet;
        newSet.push_back(L);
        CFESets.push_back(newSet);
      }
    }
    //rimuovo i set con un solo loop
    for(size_t i= 0; i<CFESets.size();){
      if(CFESets[i].size() < 2){
        CFESets.erase(CFESets.begin()+i);
      }
      else{
        ++i;
      }
    }
    //ordinamento dei set per dominanza
    for(SmallVector<Loop *, 4> &Set: CFESets){
      sortCFESet(Set, DT);
      size_t i = 0;
      while(i +1 <Set.size()){
         //check condizioni
         Loop *L0 = Set[i];
         Loop *L1 = Set[i +1];

         bool fusedloops = false;

         if(areAdjacent(L0, L1) && haveIdloopTripCount (L0, L1, SE) && dependenceAnalysis(L0, L1, DI)){
          Loop *fused = fuseLoops(L0, L1, LI);
          Set[i] = fused; 
          Set.erase(Set.begin() + i+1);
          fusedloops = true;
         }
          if(!fusedloops) 
            ++i;
      }

    }


    return PreservedAnalyses::none();
  }



llvm::PassPluginLibraryInfo getLoopFusionPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopFusionPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "loop-fusion-pass") {
                    FPM.addPass(LoopFusionPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getLoopFusionPassPluginInfo();
}
