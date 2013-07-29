//===- LazyAndersenValueInfo.cpp - value information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type holding all information for a given Value.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenValueInfo.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenRelation.h"

using namespace llvm;
using namespace llvm::lazyandersen;

ValueInfo *const ValueInfo::Nil = 0;

ValueInfo::ValueInfo(const Value *V, Map *ContainingMap)
  : V(V), ContainingMap(ContainingMap) {}

ValueInfo::~ValueInfo() {}

void ValueInfo::setAlgorithmResultSpecialCase(ValueInfoAlgorithmId Id,
    AnalysisResult *AR) {
  ResultCache.setAlgorithmResultSpecialCase(Id, AR);
}

std::list<GraphEdge> ValueInfo::getOutgoingEdges() const {
  std::list<GraphEdge> Result;
  for (HalfRelationList<SOURCE>::const_iterator
           i = getRelations<SOURCE>()->begin(),
           End = getRelations<SOURCE>()->end(); i != End; ++i) {
    const HalfRelation<SOURCE> *HR = HalfRelation<SOURCE>::from(&*i);
    Result.push_back(GraphEdge(
        Relation::getOppositeEndpoint(HR)->getValueInfo(),
        Relation::get(HR)->getRelationName()));
  }
  return Result;
}
