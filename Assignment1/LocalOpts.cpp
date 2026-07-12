#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

namespace{


    //Funzioni di servizio 

    bool idAlgebrica(bool isSomma,  BinaryOperator *BinOp, ConstantInt *opCons, Value *op){
      if(isSomma){
          if(opCons->isZero()){
            BinOp->replaceAllUsesWith(op);
            BinOp->eraseFromParent();
            return true;
          }
          return false;
      }else{
        if(opCons->isOne()){
            BinOp->replaceAllUsesWith(op);
            BinOp->eraseFromParent();
            return true;
          }
          return false;
      }
    }

    //funzioni di servizio secondo passo 2
    //moltiplicazione costante operando destro
    void createMulSRRight(Value *op, Instruction *inst, int64_t valore, bool isSub){
      //shift

      // - op1

      if(isSub){
        Instruction *NewShiftInstr = BinaryOperator::Create(Instruction::Shl, op, ConstantInt::get(inst->getType(), Log2_64(valore+1))) ; 
        NewShiftInstr->insertBefore(inst);
        Instruction *NewSubInstr = BinaryOperator::Create(Instruction::Sub, NewShiftInstr, op);
        NewSubInstr->insertBefore(inst);
        inst ->replaceAllUsesWith(NewSubInstr);

      }else{
        Instruction *NewShiftInstr = BinaryOperator::Create(Instruction::Shl, op, ConstantInt::get(inst->getType(), Log2_64(valore-1))) ; 
        NewShiftInstr->insertBefore(inst);
        Instruction *NewAddInstr = BinaryOperator::Create(Instruction::Add, NewShiftInstr, op);
        NewAddInstr->insertBefore(inst);
        inst ->replaceAllUsesWith(NewAddInstr);
      }
      inst ->eraseFromParent();
    }

    void createDivSR(Value *op, Instruction *inst, int64_t valore){
      //shift
        Instruction *NewShiftInstr = BinaryOperator::Create(Instruction::LShr, op, ConstantInt::get(inst->getType(), Log2_64(valore))) ; 
        NewShiftInstr->insertBefore(inst);
        inst ->replaceAllUsesWith(NewShiftInstr);                  
        inst ->eraseFromParent();
    }

    //------------------------------------------------------------------------------------------

    struct AlgebraicIdentityPass: PassInfoMixin<AlgebraicIdentityPass>{
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &){
            outs() <<"Ottimizzazione delle identità algebriche sulla funzione "<<F.getName() << "\n";
            
            for (auto BB = F.begin(); BB != F.end(); ++BB){
              auto I = BB->begin();
              while(I!= BB->end()){
                Instruction &Inst = *I;
                ++I;           
                if (auto *BinOp = dyn_cast<BinaryOperator>(&Inst)){
                  Value *operando1 = BinOp->getOperand(0); 
                  Value *operando2 = BinOp->getOperand(1);

                  //identità algebrica della somma  
                  if(BinOp->getOpcode()==Instruction::Add){
                    if(auto *constantZero = dyn_cast<ConstantInt>(operando2)){
                      if(idAlgebrica(true, BinOp, constantZero, operando1)){
                        continue;
                      }
                      } else if(auto *constantZero = dyn_cast<ConstantInt>(operando1)){
                        if(idAlgebrica(true, BinOp, constantZero, operando2)){
                          continue;
                      }
                    } 
                  }
             //identità algebrica della moltiplicazione
                  if(BinOp->getOpcode()==Instruction::Mul){
                    if(auto *constantOne = dyn_cast<ConstantInt>(operando2)){
                      if(idAlgebrica(false, BinOp, constantOne, operando1)){
                        outs()<<"true";
                      }
                    } else if(auto *constantOne = dyn_cast<ConstantInt>(operando1)){
                      if(idAlgebrica(false, BinOp, constantOne, operando2)){      
                        outs()<<"true";
                      }
                      }
                    } 
                  }
              }
          }
            return PreservedAnalyses::all();
        }
    };

    //------------------------------------------------------------    
    
   
                             

    struct AdvancedStrengthReductionPass: PassInfoMixin<AdvancedStrengthReductionPass>{
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &){
            outs() <<"Strength reduction avanzato sulla funzione "<<F.getName() << "\n";
            
            for(auto IterBB = F.begin(); IterBB != F.end(); ++IterBB){
              BasicBlock &BB = *IterBB;
              outs() << " Basic Block: " <<BB.getName()<<"\n";
              auto I = BB.begin();
              while(I!=BB.end()){

                Instruction &inst = *I;
                ++I;

                if(auto *op = dyn_cast<BinaryOperator>(&inst)){
                  
                  Value *op1 = op->getOperand(0);
                  Value *op2 = op->getOperand(1);


                  if(op->getOpcode() == Instruction::Mul){
                    //strength reduction (avanzato) sulla moltiplicazione
                    //caso: costante nell'operando destro 
                    if(auto *costante=dyn_cast<ConstantInt>(op2)){
                      int64_t valore = costante -> getSExtValue();
                      if(isPowerOf2_64(valore + 1)){
                        createMulSRRight(op1, &inst, valore, true);
                      }else if(isPowerOf2_64(valore - 1)){
                        createMulSRRight(op1, &inst, valore, false);
                      } 
                    }
                    //caso: costante nell'operando sinistro
                    if(auto *costante=dyn_cast<ConstantInt>(op1)){
                      int64_t valore = costante -> getSExtValue();
                      if(isPowerOf2_64(valore + 1)){
                        createMulSRRight(op2, &inst, valore, true);
                      }else if(isPowerOf2_64(valore - 1)){
                        createMulSRRight(op2, &inst, valore, false);
                      }

                    }
                  }else if(op->getOpcode() == Instruction::UDiv){
                    //funziona sulle unsigned divisions. da guardare anche i numeri con segni
                    if(auto *costante=dyn_cast<ConstantInt>(op2)){
                      int64_t valore = costante -> getSExtValue();
                      if(isPowerOf2_64(valore)){
                        createDivSR(op1, &inst, valore);
                      }
                    }
                  }

              }

            }
          }


            return PreservedAnalyses::all();
        }
    };

    
    //---------------------------------------------------------------------
    struct MultiInstrOptPass: PassInfoMixin<MultiInstrOptPass>{
            PreservedAnalyses run(Function &F, FunctionAnalysisManager &){
            outs() <<"Ottimizzazione multi-instruction "<<F.getName() << "\n";
            
            for(auto IterBB = F.begin(); IterBB != F.end(); ++IterBB){
              BasicBlock &BB = *IterBB;
              outs() << " Basic Block: " <<BB.getName()<<"\n";
              auto I = BB.begin();

              while(I!=BB.end()){

                Instruction &inst = *I;
                ++I;
                if (auto *binaryoperation = dyn_cast<BinaryOperator>(&inst)){
                BinaryOperator &binop = *binaryoperation;
                Value *op1 = binop.getOperand(0);
                Value *op2 = binop.getOperand(1);
                if(binop.getOpcode()==Instruction::Sub){      
                  if(auto *cSub = dyn_cast<ConstantInt>(op2)){
                    int64_t valSub = cSub->getSExtValue();
                    if(auto *op1Inst=dyn_cast<Instruction>(op1)){
                      if(op1Inst->getOpcode()==Instruction::Add){
                        Value *opAdd1 = op1Inst->getOperand(0);
                        Value *opAdd2 = op1Inst->getOperand(1);

                        if(auto *cAdd = dyn_cast<ConstantInt>(opAdd2)){
                          int64_t valAdd = cAdd->getSExtValue();
                          if(valSub == valAdd){
                            binop.replaceAllUsesWith(opAdd1);
                            binop.eraseFromParent();
                          }
                        }else if(auto *cAdd = dyn_cast<ConstantInt>(opAdd1)){
                          int64_t valAdd = cAdd->getSExtValue();
                          if(valSub == valAdd){
                            binop.replaceAllUsesWith(opAdd2);
                            binop.eraseFromParent();
                        }
                      }
                        
                      }
                    }
                  }
              }else if(binop.getOpcode() == Instruction::Add){
                if(auto *cAdd = dyn_cast<ConstantInt>(op2)){
                  int64_t valAdd = cAdd ->getSExtValue();
                  if(auto *op1Inst = dyn_cast<Instruction>(op1)){
                    if (op1Inst->getOpcode() == Instruction::Sub){
                      Value *opSub1 = op1Inst->getOperand(0);
                      Value *opSub2 = op1Inst->getOperand(1);

                      if(auto *cSub =dyn_cast<ConstantInt>(opSub2)){
                        int64_t valSub = cSub->getSExtValue();
                        if(valAdd == valSub){
                          binop.replaceAllUsesWith(opSub1);
                          binop.eraseFromParent();
                        } 
                      }
                    }
                  }
                }else if(auto *cAdd = dyn_cast<ConstantInt>(op1)){
                  int64_t valAdd = cAdd ->getSExtValue();
                  if(auto *op1Inst = dyn_cast<Instruction>(op2)){
                    if (op1Inst->getOpcode() == Instruction::Sub){
                      Value *opSub1 = op1Inst->getOperand(0);
                      Value *opSub2 = op1Inst->getOperand(1);

                      if(auto *cSub =dyn_cast<ConstantInt>(opSub2)){
                        int64_t valSub = cSub->getSExtValue();
                        if(valAdd == valSub){
                          binop.replaceAllUsesWith(opSub1);
                          binop.eraseFromParent();
                        } 
                      }
                    }
                  }

                }
              }}

                
              }


            }    

            return PreservedAnalyses::all();
        
      }
    };
}

//registrazione dei passi nel new pass manager, uno alla volta per poterli usare separatamente
llvm::PassPluginLibraryInfo getLocalOptsPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "LocalOpts", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {

                  if (Name == "algebraic-id") {
                    FPM.addPass(AlgebraicIdentityPass());
                    return true;
                  }
                  if (Name == "advanced-sr") {
                    FPM.addPass(AdvancedStrengthReductionPass());
                    return true;
                  }
                  if (Name == "multi-inst") {
                    FPM.addPass(MultiInstrOptPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return getLocalOptsPluginInfo();
}
