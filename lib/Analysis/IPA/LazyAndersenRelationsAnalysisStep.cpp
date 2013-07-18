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

template<EdgeEndpointType Endpoint>
RelationsAnalysisStep<Endpoint>::RelationsAnalysisStep(ValueInfo *VI)
  : RelationsAnalysisStepBase(VI->getRelations<Endpoint>()) {}

template<EdgeEndpointType Endpoint>
AnalysisResult *RelationsAnalysisStep<Endpoint>::analyzeHalfRelation(
    HalfRelationBase *HR) {
  return analyzeRelation(Relation::get(HalfRelation<Endpoint>::from(HR)));
}

template class RelationsAnalysisStep<SOURCE>;
template class RelationsAnalysisStep<DESTINATION>;
