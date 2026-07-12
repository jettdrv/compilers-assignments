/**
  * @file LoopPass.cpp
  * @brief Implementazione del pass per la Loop-Invariant Code Motion
 */


#include <algorithm>
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Dominators.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/ADT/SmallPtrSet.h"





using namespace llvm;


namespace {
//FUNZIONI DI SERVIZIO 

/// @brief Controlla se un'istruzione è loop-invariant
/// @param L
/// @param I  puntatore all'istruzione in esaminazione
/// @param LoopInvariantIns riferimento al set contenente le istruzioni già marcate come loop-invariant 
/// @return true se l'istruzione è loop-invariant, false altrimenti

static bool isInstructionInvariant(Loop *L, Instruction *I, SmallPtrSetImpl<Instruction*> &LoopInvariantIns){
  //solo le istruzioni che non leggono o scrivono dalla memoria sono candidate alla code motion
    if(!(I->isBinaryOp() || I->isShift() || isa<SelectInst>(I) || isa<CastInst>(I) || isa<GetElementPtrInst>(I))){
      return false;
    }

    //controllo ogni operando. Le condizioni da soddisfare affinché l'istruzione venga considerata loop-invariant sono:
    // 1) operandi costanti, argomenti della funzione o valori globali
    // 2) operandi definiti all'esterno del loop 
    // 3) operandi definiti all'interno del loop ma le istruzioni che li definiscono sono loop-invariant
    for(auto op = I->op_begin(); op != I->op_end(); ++op){
        Value *opVal = op->get();
          if(!(isa<Constant>(opVal) || isa<Argument>(opVal) || isa<GlobalValue>(opVal))){

            if(Instruction *opDef = dyn_cast<Instruction>(opVal)){
              bool isDefOutsideLoop = !(L->contains(opDef->getParent()));
              bool isAlreadyInvariant = LoopInvariantIns.count(opDef);

              if(!isDefOutsideLoop && !isAlreadyInvariant){
                  return false;
              }
          }else{
            return false;
          }
        } 
    }

          return LoopInvariantIns.insert(I).second;
}




/// @brief Verifica se l'istruzione candidata alla code motion domina tutte le uscite del loop
/// @param I puntatore all'istruzione candidata
/// @param DT riferimento alla Dominator Tree 
/// @param exitBlocks vettore contenente i blocchi di uscita del loop 
/// @return true se l'istruzione domina tutte le uscite, false altrimenti

static bool dominatesAllExits(Instruction *I, DominatorTree &DT, const SmallVectorImpl<BasicBlock*> &exitBlocks){
  for(BasicBlock *Exit : exitBlocks){
    if(!DT.dominates(I->getParent(), Exit))
      return false;
  }
  return true;
}


/// @brief Funzione che controlla se il valore definito dall'istruzione corrente non viene mai utilizzato al di fuori del loop. 
/// @details Se l'istruzione non ha usi vivi all'esterno del loop, potremmo procedere con il code motion anche se l'istruzione non domina tutte le uscite del loop.
/// @param I puntatore all'istruzione
/// @param L puntatore al loop corrente
/// @return true se il valore non viene mai usato fuori dal loop (dead at exit), false altrimenti


static bool isDeadAtLoopExit(Instruction *I, Loop *L) {
    for (User *U : I->users()) {
        Instruction *UI = dyn_cast<Instruction>(U);
        if (!UI)
            continue;

        if (PHINode *PN = dyn_cast<PHINode>(UI)) {
            for (unsigned i = 0; i < PN->getNumIncomingValues(); ++i) {
                if (PN->getIncomingValue(i) == I &&
                    !L->contains(PN->getIncomingBlock(i)))
                    return false;  
            }
        } else {
            if (!L->contains(UI->getParent()))
                return false;   
        }
    }
    return true;   
}

/// @brief Verifica che tutti gli operandi definiti all'interno del loop, siano già stati spostati nel preheader
/// @param I puntatore all'istruzione considerata per lo spostamento
/// @param L il loop corrente
/// @return true se gli operandi sono già stati spostati, false altrimenti

static bool invOperandsMoved(Instruction *I, Loop *L){
  for(auto op= I->op_begin(); op!= I->op_end(); ++op){
    if(Instruction *opDef =dyn_cast<Instruction>(op->get())){
      if(L->contains(opDef->getParent()))
        return false;
    }
  }
  return true;
}


/// @brief Controlla se è safe effettuare la code motion (o code hoisting)
/// @param I puntatore all'istruzione candidata
/// @param DT riferimento alla Dominator Tree 
/// @param exitBlocks vettore contenente i blocchi di uscita del loop 
/// @return true se l'istruzione può essere spostata nel preheader, false altrimenti

static bool isInstructionHoistable(Loop *L, Instruction *I, DominatorTree &DT, const SmallVectorImpl<BasicBlock*> &exitBlocks){
  if ((dominatesAllExits(I, DT, exitBlocks) || isDeadAtLoopExit(I, L)) && invOperandsMoved(I, L)) {
    return true;
  }
  return false;
}



//-------------------------------------------------------------------------------

struct LoopPass : PassInfoMixin<LoopPass> {

/// @brief Function pass che effettua tutte le trasformazioni sui loop
/// @param F Funzione su cui applicare i passi di trasformazione
/// @param AM New pass manager per funzioni
/// @return Un oggetto PreservedAnalyses che indica quali analisi sono rimaste valide dopo il pass

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    LoopInfo &LI = AM.getResult<LoopAnalysis>(F);
    DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);

    //verifica se il CFG corrente contiene loop
      if(LI.begin() == LI.end()){
        outs()<<"Il CFG non contiene loops \n";
        return PreservedAnalyses::all();
      }
 
      for(Loop *L: llvm::reverse(LI.getLoopsInPreorder())){
        //PRIMA PARTE DEL PASS: trovo le istruzioni loop-invariant. Uso una struttura dati fornita da llvm, SmallPtrSet, che è un insieme di puntatori inizialmente di dimensione 16. Se dovessi inserire più istruzioni, la memoria viene allocata dinamicamente.
        SmallPtrSet<Instruction*, 16> LoopInvariantIns;
        bool changed = true;
        while(changed){
          changed = false;
          for(BasicBlock *BB: L->blocks()){
            for (Instruction &I: *BB){

              //l'istruzione non è presente nel set, check isLoopInvariant
              if(!LoopInvariantIns.count(&I)){
                if(isInstructionInvariant(L, &I, LoopInvariantIns))
                  changed = true;
              }
            }
            
          }
        }
        outs()<<"DEBUG:: \n";
        if(LoopInvariantIns.empty()){
              outs()<<"vettore vuoto \n";
            } else{
              for(Instruction *I: LoopInvariantIns){
                outs()<<"INSTRUCTION LOOP INVARIANT: "<<*I <<"\n";
              }
            }

      
      //SECONDA PARTE: controllo delle condizioni di code motion
      //verifica del preheader e dei blocchi di uscita
      BasicBlock *preheader = L->getLoopPreheader();
      BasicBlock *header = L->getHeader();
      SmallVector<BasicBlock*, 4> exitBlocks;
      L->getExitBlocks(exitBlocks);
      

      if(!preheader){
        outs()<<"Nessun preheader dove spostare le istruzioni loop invariant \n";
        continue;
      }
      auto pos = preheader->getTerminator();


    // sposta le istruzioni candidate, in ordine di dominanza, controllando le condizioni
      for (auto *DTN : depth_first(DT.getNode(header))) {
        BasicBlock *BB = DTN->getBlock();
        if (L->contains(BB)){
          for (Instruction &I : make_early_inc_range(*BB)) {
              if (LoopInvariantIns.count(&I) && isInstructionHoistable(L, &I, DT, exitBlocks)) {
                  outs() << "Sposto nel preheader: " << I << "\n";
                  I.moveBefore(pos);
              }
            }
        }
        }

      }

    return PreservedAnalyses::none();
  }
  };

}


llvm::PassPluginLibraryInfo getLoopPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LoopPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "loop-pass") {
                    FPM.addPass(LoopPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getLoopPassPluginInfo();
}
