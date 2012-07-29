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

#include "LazyAndersenRelation-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *DependsOnRelation::getRelationName() const {
  return "depends on";
}

AnalysisResult *DependsOnRelation::analyzePointsToSet() const {
  return getValueInfo<INCOMING>()->getAlgorithmResult<POINTS_TO_SET>();
}
