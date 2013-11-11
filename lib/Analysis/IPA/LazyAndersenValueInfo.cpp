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
#include "LazyAndersenRecursiveEnumerate.h"
#include "LazyAndersenValuePrinter.h"
#include "llvm/ADT/STLExtras.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

ValueInfo *const ValueInfo::Nil = 0;

ValueInfo::ValueInfo(const Value *V) : V(V) {}

ValueInfo::~ValueInfo() {
  DeleteContainerSeconds(Results);
}

GraphEdgeDeque ValueInfo::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  for (ResultsMapTy::const_iterator i = Results.begin(); i != Results.end();
       ++i) {
    const char *Id = i->first;
    AnalysisResult *AR = i->second;
    std::ostringstream OSS;
    OSS << "PointsTo(" << Id << ')';
    Result.push_back(GraphEdge(AR, OSS.str()));
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

AnalysisResult *ValueInfo::getOrCreateAlgorithmResult(AlgorithmIdTy Id,
    AlgorithmFn Fn) {
  AnalysisResult *&AR = Results[Id];
  if (!AR) {
    AR = (*Fn)(this);
    assert(AR);
  }
  return AR;
}

AnalysisResult *ValueInfo::getAlgorithmResultOrNull(AlgorithmIdTy Id) const {
  ResultsMapTy::const_iterator i = Results.find(Id);
  if (i == Results.end()) {
    return 0;
  }
  return i->second;
}

void ValueInfo::addInstructionAnalysisWorkInternal(AlgorithmIdTy Id1,
    AlgorithmFn Fn1, ValueInfo *that, AlgorithmIdTy Id2,
    AlgorithmFn Fn2) {
  getOrCreateAlgorithmResult(Id1, Fn1)->addWork(
      new RecursiveEnumerate(that->getOrCreateAlgorithmResult(Id2, Fn2)));
}
