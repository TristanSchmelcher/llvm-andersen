//===- LazyAndersenRelationsAnalysisStep.cpp - analysis classes -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the template type for relations analysis.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenRelationsAnalysisStep.h"

#include "LazyAndersenRelation.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace lazyandersen;

template<RelationDirection Direction>
RelationsAnalysisStep<Direction>::RelationsAnalysisStep(ValueInfo *VI)
  : RelationsAnalysisStepBase(VI->getRelations<Direction>()) {}

template<RelationDirection Direction>
AnalysisResult *RelationsAnalysisStep<Direction>::analyzeHalfRelation(
    HalfRelationBase *HR) {
  return analyzeRelation(Relation::get(HalfRelation<Direction>::from(HR)));
}

template class RelationsAnalysisStep<INCOMING>;
template class RelationsAnalysisStep<OUTGOING>;
