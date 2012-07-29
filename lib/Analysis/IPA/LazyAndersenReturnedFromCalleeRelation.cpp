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

#include "LazyAndersenAlgorithmResultCache-inl.h"
#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultAlgorithms.h"
#include "LazyAndersenRelation-inl.h"
#include "LazyAndersenValueInfoAlgorithms.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *ReturnedFromCalleeRelation::getRelationName() const {
  return "returned from callee";
}

AnalysisResult *ReturnedFromCalleeRelation::analyzePointsToSet() const {
  return getValueInfo<INCOMING>()->getAlgorithmResult<POINTS_TO_SET>()
      ->getAlgorithmResult<RETURN_VALUE_POINTS_TO_SET>();
}
