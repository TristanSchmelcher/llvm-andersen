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
#include "LazyAndersenValuePrinter.h"

#include <sstream>

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

GraphEdgeDeque ValueInfo::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  for (HalfRelationList<SOURCE>::const_iterator
           i = getRelations<SOURCE>()->begin(),
           End = getRelations<SOURCE>()->end(); i != End; ++i) {
    const HalfRelation<SOURCE> *HR = HalfRelation<SOURCE>::from(&*i);
    const Relation *R = Relation::get(HR);
    std::ostringstream OSS;
    OSS << R << " " << R->getRelationName();
    Result.push_back(GraphEdge(
        Relation::getOppositeEndpoint(HR)->getValueInfo(),
        OSS.str()));
  }
  for (int i = 0; i < NUM_VALUE_INFO_ALGORITHMS; ++i) {
    AnalysisResult *AR = ResultCache.getAlgorithmResultNoCreate(
        static_cast<ValueInfoAlgorithmId>(i));
    if (!AR) continue;
    Result.push_back(GraphEdge(AR, ValueInfoAlgorithmNames[i]));
  }
  return Result;
}

std::string ValueInfo::getNodeLabel() const {
  static const size_t MaxPrintedSize = 16;
  return prettyPrintValue(getValue(), MaxPrintedSize);
}

bool ValueInfo::isNodeHidden() const {
  return false;
}
