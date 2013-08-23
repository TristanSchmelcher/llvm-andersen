//===- LazyAndersenData.cpp - all analysis data ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type holding all analysis information.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenData.h"

#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

GraphEdgeDeque LazyAndersenData::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  for (ValueInfo::Map::const_iterator
           i = ValueInfos.begin(), End = ValueInfos.end(); i != End; ++i) {
    if (!i->second.getPtr() || i->first != i->second->getValue()) continue;
    // No edge label needed because this edge will not be printed.
    Result.push_back(GraphEdge(i->second.getPtr(), std::string()));
  }
  return Result;
}

std::string LazyAndersenData::getNodeLabel() const {
  llvm_unreachable("LazyAndersenData should not be emitted in the graph");
  return std::string();
}

bool LazyAndersenData::isNodeHidden() const {
  return true;
}
