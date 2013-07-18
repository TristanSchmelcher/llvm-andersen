//===- LazyAndersenStoredToRelation.cpp - loaded-from relation ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a stored-to relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenStoredToRelation.h"

#include "LazyAndersenAnalysisResult-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *StoredToRelation::getRelationName() const {
  return "stored to";
}

AnalysisResult *StoredToRelation::analyzePointsToSet() const {
  // No effect on points-to set.
  return 0;
}

AnalysisResult *StoredToRelation::analyzeOutgoingReversePointsToSet() const {
  return getValueInfo<DESTINATION>()->getAlgorithmResult<POINTS_TO_SET>()
      ->getAlgorithmResult<CONTENT_REVERSE_POINTS_TO_SET>();
}

AnalysisResult *StoredToRelation::analyzeIncomingReversePointsToSet() const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *StoredToRelation::analyzeStoredValuesPointsToSet()
    const {
  return getValueInfo<SOURCE>()->getAlgorithmResult<POINTS_TO_SET>();
}

AnalysisResult *
StoredToRelation::analyzeLoadedValuesReversePointsToSet() const {
  // No effect on loaded values reverse points-to set.
  return 0;
}

AnalysisResult *StoredToRelation::analyzeArgumentsPointsToSet() const {
  // No effect on arguments points-to set.
  return 0;
}

AnalysisResult *StoredToRelation::analyzeArgumentsReversePointsToSet() const {
  // No effect on arguments reverse points-to set.
  return 0;
}

AnalysisResult *StoredToRelation::analyzeReturnValuePointsToSet() const {
  // No effect on return value points-to set.
  return 0;
}

AnalysisResult *StoredToRelation::analyzeReturnValueReversePointsToSet() const {
  // No effect on return value reverse points-to set.
  return 0;
}
