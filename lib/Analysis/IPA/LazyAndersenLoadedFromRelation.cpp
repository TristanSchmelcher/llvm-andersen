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
  return getValueInfo<INCOMING>()->getAlgorithmResult<POINTS_TO_SET>()
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
