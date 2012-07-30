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
#include "LazyAndersenRelation-inl.h"
#include "LazyAndersenValueInfo-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *ArgumentFromCallerRelation::getRelationName() const {
  return "argument from caller";
}

AnalysisResult *ArgumentFromCallerRelation::analyzePointsToSet() const {
  return getValueInfo<INCOMING>()
      ->getAlgorithmResult<REVERSE_POINTS_TO_SET>()
          ->getAlgorithmResult<ARGUMENT_POINTS_TO_SET>();
}
