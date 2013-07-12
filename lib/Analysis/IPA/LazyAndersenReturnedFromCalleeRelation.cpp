//===- LazyAndersenReturnedFromCalleeRelation.cpp - returned-from relation -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a returned-from-callee relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReturnedFromCalleeRelation.h"

#include "LazyAndersenAnalysisResult-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *ReturnedFromCalleeRelation::getRelationName() const {
  return "returned from callee";
}

AnalysisResult *ReturnedFromCalleeRelation::analyzePointsToSet() const {
  return getValueInfo<INCOMING>()->getAlgorithmResult<POINTS_TO_SET>()
      ->getAlgorithmResult<RETURN_VALUE_POINTS_TO_SET>();
}

AnalysisResult *ReturnedFromCalleeRelation::analyzeOutgoingReversePointsToSet()
    const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *ReturnedFromCalleeRelation::analyzeIncomingReversePointsToSet()
    const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *ReturnedFromCalleeRelation::analyzeStoredValuesPointsToSet()
    const {
  // No effect on stored values points-to set.
  return 0;
}

AnalysisResult *
ReturnedFromCalleeRelation::analyzeLoadedValuesReversePointsToSet() const {
  // No effect on loaded values reverse points-to set.
  return 0;
}

AnalysisResult *
ReturnedFromCalleeRelation::analyzeArgumentsPointsToSet() const {
  // No effect on arguments points-to set.
  return 0;
}

AnalysisResult *
ReturnedFromCalleeRelation::analyzeArgumentsReversePointsToSet() const {
  // No effect on arguments reverse points-to set.
  return 0;
}

AnalysisResult *
ReturnedFromCalleeRelation::analyzeReturnValuePointsToSet() const {
  // No effect on return value points-to set.
  return 0;
}

AnalysisResult *
ReturnedFromCalleeRelation::analyzeReturnValueReversePointsToSet() const {
  return getValueInfo<OUTGOING>()->getAlgorithmResult<REVERSE_POINTS_TO_SET>();
}
