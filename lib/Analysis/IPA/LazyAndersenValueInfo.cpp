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
#include "LazyAndersenValuePrinter.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

ValueInfo *const ValueInfo::Nil = 0;

ValueInfo::ValueInfo(const Value *V) : V(V) {}

ValueInfo::~ValueInfo() {}

GraphEdgeDeque ValueInfo::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  for (AnalysisResultCacheTy::ResultsMapTy::const_iterator i =
           AnalysisResultCacheTy::getResults().begin();
       i != AnalysisResultCacheTy::getResults().end(); ++i) {
    const char *Id = i->first;
    AnalysisResult *AR = i->second;
    Result.push_back(GraphEdge(AR, Id));
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
