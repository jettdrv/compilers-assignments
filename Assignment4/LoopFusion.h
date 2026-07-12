/**
  * @file LoopFusion.h
  * @brief Header file del pass di Loop Fusion
 */

#ifndef TRANSFORM_LOOPFUSION_H
#define  TRANSFORM_LOOPFUSION_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"


namespace llvm{

    struct LoopFusionPass : PassInfoMixin<LoopFusionPass> {
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
    };
}

#endif