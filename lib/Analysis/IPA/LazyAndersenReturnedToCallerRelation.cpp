//===- LazyAndersenReturnedToCallerRelation.cpp - returned-to relation ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a returned-to-caller relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReturnedToCallerRelation.h"

#include "LazyAndersenAnalysisResult-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *ReturnedToCallerRelation::getRelationName() const {
  return "returned to caller";
}

AnalysisResult *ReturnedToCallerRelation::analyzePointsToSet() const {
  // No effect on points-to set.
  return 0;
}

AnalysisResult *ReturnedToCallerRelation::analyzeOutgoingReversePointsToSet()
    const {
  return getValueInfo<DESTINATION>()
      ->getAlgorithmResult<REVERSE_POINTS_TO_SET>()
          ->getAlgorithmResult<RETURN_VALUE_REVERSE_POINTS_TO_SET>();
}

AnalysisResult *ReturnedToCallerRelation::analyzeIncomingReversePointsToSet()
    const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *ReturnedToCallerRelation::analyzeStoredValuesPointsToSet()
    const {
  // No effect on stored values points-to set.
  return 0;
}

AnalysisResult *
ReturnedToCallerRelation::analyzeLoadedValuesReversePointsToSet() const {
  // No effect on loaded values reverse points-to set.
  return 0;
}

AnalysisResult *ReturnedToCallerRelation::analyzeArgumentsPointsToSet() const {
  // No effect on arguments points-to set.
  return 0;
}

AnalysisResult *
ReturnedToCallerRelation::analyzeArgumentsReversePointsToSet() const {
  // No effect on arguments reverse points-to set.
  return 0;
}

AnalysisResult *
ReturnedToCallerRelation::analyzeReturnValuePointsToSet() const {
  return getValueInfo<SOURCE>()->getAlgorithmResult<POINTS_TO_SET>();
}

AnalysisResult *
ReturnedToCallerRelation::analyzeReturnValueReversePointsToSet() const {
  // No effect on return value reverse points-to set.
  return 0;
}
