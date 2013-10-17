//===- LazyAndersenArgumentFromCallerRelation.cpp - argument-from relation ===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a argument-from-caller relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenArgumentFromCallerRelation.h"

#include "LazyAndersenAnalysisResult-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *ArgumentFromCallerRelation::getRelationName() const {
  return "argument from caller";
}

AnalysisResult *ArgumentFromCallerRelation::analyzePointsToSet() const {
  return getValueInfo<DESTINATION>()
      ->getAlgorithmResult<ARGUMENT_POINTS_TO_SET>();
}

AnalysisResult *ArgumentFromCallerRelation::analyzeOutgoingReversePointsToSet()
    const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *ArgumentFromCallerRelation::analyzeIncomingReversePointsToSet()
    const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *ArgumentFromCallerRelation::analyzeStoredValuesPointsToSet()
    const {
  // No effect on stored values points-to set.
  return 0;
}

AnalysisResult *
ArgumentFromCallerRelation::analyzeLoadedValuesReversePointsToSet() const {
  // No effect on loaded values reverse points-to set.
  return 0;
}

AnalysisResult *
ArgumentFromCallerRelation::analyzeArgumentsPointsToSet() const {
  // No effect on arguments points-to set.
  return 0;
}

AnalysisResult *
ArgumentFromCallerRelation::analyzeArgumentsReversePointsToSet() const {
  return getValueInfo<SOURCE>()->getAlgorithmResult<REVERSE_POINTS_TO_SET>();
}

AnalysisResult *
ArgumentFromCallerRelation::analyzeReturnValuePointsToSet() const {
  // No effect on return value points-to set.
  return 0;
}

AnalysisResult *
ArgumentFromCallerRelation::analyzeReturnValueReversePointsToSet() const {
  // No effect on return value reverse points-to set.
  return 0;
}
