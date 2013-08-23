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

#include <sstream>

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

template<EdgeEndpointType Endpoint>
GraphEdgeDeque RelationsAnalysisStep<Endpoint>::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  std::ostringstream OSS;
  if (i != List->end()) {
    OSS << Relation::get(HalfRelation<Endpoint>::from(&*i));
  } else {
    OSS << "end";
  }
  OSS << " of " << EdgeEndpointTraitsBase<Endpoint>::EdgeEndpointTypeShortName;
  Result.push_back(GraphEdge(
      ValueInfo::get(HalfRelationList<Endpoint>::from(List)), OSS.str()));
  return Result;
}

template class RelationsAnalysisStep<SOURCE>;
template class RelationsAnalysisStep<DESTINATION>;
