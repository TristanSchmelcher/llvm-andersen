//===- LazyAndersenDependsOnRelation.cpp - depends-on relation ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a depends-on relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenDependsOnRelation.h"

#include "LazyAndersenAnalysisResult-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *DependsOnRelation::getRelationName() const {
  return "depends on";
}

AnalysisResult *DependsOnRelation::analyzePointsToSet() const {
  return getValueInfo<DESTINATION>()->getAlgorithmResult<POINTS_TO_SET>();
}

AnalysisResult *DependsOnRelation::analyzeOutgoingReversePointsToSet()
    const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *DependsOnRelation::analyzeIncomingReversePointsToSet()
    const {
  return getValueInfo<SOURCE>()->getAlgorithmResult<REVERSE_POINTS_TO_SET>();
}

AnalysisResult *DependsOnRelation::analyzeStoredValuesPointsToSet()
    const {
  // No effect on stored values points-to set.
  return 0;
}

AnalysisResult *
DependsOnRelation::analyzeLoadedValuesReversePointsToSet() const {
  // No effect on loaded values reverse points-to set.
  return 0;
}

AnalysisResult *DependsOnRelation::analyzeArgumentsPointsToSet() const {
  // No effect on arguments points-to set.
  return 0;
}

AnalysisResult *DependsOnRelation::analyzeArgumentsReversePointsToSet() const {
  // No effect on arguments reverse points-to set.
  return 0;
}

AnalysisResult *DependsOnRelation::analyzeReturnValuePointsToSet() const {
  // No effect on return value points-to set.
  return 0;
}

AnalysisResult *
DependsOnRelation::analyzeReturnValueReversePointsToSet() const {
  // No effect on return value reverse points-to set.
  return 0;
}
