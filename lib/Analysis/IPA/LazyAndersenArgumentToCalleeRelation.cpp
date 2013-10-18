//===- LazyAndersenArgumentToCalleeRelation.cpp - argument-to relation ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a argument-to-callee relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenArgumentToCalleeRelation.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *ArgumentToCalleeRelation::getRelationName() const {
  return "argument to callee";
}

AnalysisResult *ArgumentToCalleeRelation::analyzePointsToSet() const {
  // No effect on points-to set.
  return 0;
}

AnalysisResult *ArgumentToCalleeRelation::analyzeOutgoingReversePointsToSet()
    const {
  return getValueInfo<DESTINATION>()
      ->getAlgorithmResult<ARGUMENT_REVERSE_POINTS_TO_SET>();
}

AnalysisResult *ArgumentToCalleeRelation::analyzeIncomingReversePointsToSet()
    const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *ArgumentToCalleeRelation::analyzeStoredValuesPointsToSet()
    const {
  // No effect on stored values points-to set.
  return 0;
}

AnalysisResult *
ArgumentToCalleeRelation::analyzeLoadedValuesReversePointsToSet() const {
  // No effect on loaded values reverse points-to set.
  return 0;
}

AnalysisResult *ArgumentToCalleeRelation::analyzeArgumentsPointsToSet() const {
  return getValueInfo<SOURCE>()->getAlgorithmResult<POINTS_TO_SET>();
}

AnalysisResult *
ArgumentToCalleeRelation::analyzeArgumentsReversePointsToSet() const {
  // No effect on arguments reverse points-to set.
  return 0;
}

AnalysisResult *
ArgumentToCalleeRelation::analyzeReturnValuePointsToSet() const {
  // No effect on return value points-to set.
  return 0;
}

AnalysisResult *
ArgumentToCalleeRelation::analyzeReturnValueReversePointsToSet() const {
  // No effect on return value reverse points-to set.
  return 0;
}
