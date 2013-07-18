//===- LazyAndersenLoadedFromRelation.cpp - loaded-from relation ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a loaded-from relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenLoadedFromRelation.h"

#include "LazyAndersenAnalysisResult-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *LoadedFromRelation::getRelationName() const {
  return "loaded from";
}

AnalysisResult *LoadedFromRelation::analyzePointsToSet() const {
  return getValueInfo<DESTINATION>()->getAlgorithmResult<POINTS_TO_SET>()
      ->getAlgorithmResult<CONTENT_POINTS_TO_SET>();
}

AnalysisResult *LoadedFromRelation::analyzeOutgoingReversePointsToSet() const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *LoadedFromRelation::analyzeIncomingReversePointsToSet() const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *LoadedFromRelation::analyzeStoredValuesPointsToSet()
    const {
  // No effect on stored values points-to set.
  return 0;
}

AnalysisResult *
LoadedFromRelation::analyzeLoadedValuesReversePointsToSet() const {
  return getValueInfo<SOURCE>()->getAlgorithmResult<REVERSE_POINTS_TO_SET>();
}

AnalysisResult *LoadedFromRelation::analyzeArgumentsPointsToSet() const {
  // No effect on arguments points-to set.
  return 0;
}

AnalysisResult *LoadedFromRelation::analyzeArgumentsReversePointsToSet() const {
  // No effect on arguments reverse points-to set.
  return 0;
}

AnalysisResult *LoadedFromRelation::analyzeReturnValuePointsToSet() const {
  // No effect on return value points-to set.
  return 0;
}

AnalysisResult *
LoadedFromRelation::analyzeReturnValueReversePointsToSet() const {
  // No effect on return value reverse points-to set.
  return 0;
}
