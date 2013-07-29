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

using namespace llvm;
using namespace llvm::lazyandersen;

std::list<GraphEdge> LazyAndersenData::getOutgoingEdges() const {
  std::list<GraphEdge> Result;
  for (ValueInfo::Map::const_iterator
           i = ValueInfos.begin(), End = ValueInfos.end(); i != End; ++i) {
    if (!i->second.getPtr() || i->first != i->second->getValue()) continue;
    Result.push_back(GraphEdge(i->second.getPtr(), "hidden edge"));
  }
  return Result;
}
