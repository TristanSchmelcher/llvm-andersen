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
  return getValueInfo<INCOMING>()->getAlgorithmResult<POINTS_TO_SET>()
      ->getAlgorithmResult<CONTENT_REVERSE_POINTS_TO_SET>();
}

AnalysisResult *StoredToRelation::analyzeIncomingReversePointsToSet() const {
  // No effect on reverse points-to set.
  return 0;
}

AnalysisResult *StoredToRelation::analyzeStoredValuesPointsToSet()
    const {
  return getValueInfo<OUTGOING>()->getAlgorithmResult<POINTS_TO_SET>();
}
