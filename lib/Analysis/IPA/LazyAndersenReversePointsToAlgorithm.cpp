//===- LazyAndersenReversePointsToAlgorithm.cpp - reverse points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenArgumentReversePointsToAlgorithm.h"
#include "LazyAndersenContentReversePointsToAlgorithm.h"
#include "LazyAndersenReturnValueReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char ReversePointsToAlgorithm::ID[] = "reverse points-to";

AnalysisResult *ReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addValueInfo(VI);
  return AR;
}

void ReversePointsToAlgorithm::RelationHandler<ARGUMENT_TO_CALLEE>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->addInstructionAnalysisWork<ReversePointsToAlgorithm,
      ArgumentReversePointsToAlgorithm>(Dst);
}

void ReversePointsToAlgorithm::RelationHandler<DEPENDS_ON>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Dst->addInstructionAnalysisWork<ReversePointsToAlgorithm,
      ReversePointsToAlgorithm>(Src);
}

void ReversePointsToAlgorithm::RelationHandler<RETURNED_TO_CALLER>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->addInstructionAnalysisWork<ReversePointsToAlgorithm,
      ReturnValueReversePointsToAlgorithm>(Dst);
}

void ReversePointsToAlgorithm::RelationHandler<STORED_TO>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->addInstructionAnalysisWork<ReversePointsToAlgorithm,
      ContentReversePointsToAlgorithm>(Dst);
}
