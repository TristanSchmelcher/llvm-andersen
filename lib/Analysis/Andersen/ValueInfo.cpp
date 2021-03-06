//===- ValueInfo.cpp - value information ----------------------------------===//
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

#include "ValueInfo.h"

#include "AlgorithmId.h"
#include "AnalysisResult.h"
#include "DebugInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"

#include <sstream>

namespace llvm {
namespace andersen_internal {

ValueInfo::ValueInfo(const Value *V) : V(V) {}

ValueInfo::~ValueInfo() {
  DeleteContainerSeconds(Results);
}

GraphEdgeDeque ValueInfo::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  for (ResultsMapTy::const_iterator i = Results.begin(), End = Results.end();
       i != End; ++i) {
    const AlgorithmId *Id = i->first;
    AnalysisResult *AR = i->second;
    std::ostringstream OSS;
    OSS << "PointsTo(";
    {
      raw_os_ostream OS(OSS);
      Id->printAlgorithmName(OS);
    }
    OSS << ')';
    Result.push_back(GraphEdge(AR, OSS.str()));
  }
  return Result;
}

void ValueInfo::printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const {
  DI.printValueInfoName(this, OS);
}

bool ValueInfo::isNodeHidden() const {
  return false;
}

void ValueInfo::fillDebugInfo(DebugInfoFiller *DIF) const {
  for (ResultsMapTy::const_iterator i = Results.begin(), End = Results.end();
       i != Results.end(); ++i) {
    DIF->fill(i->second, this, i->first);
  }
}

void ValueInfo::writeEquations(const DebugInfo &DI, raw_ostream &OS) const {
  for (ResultsMapTy::const_iterator i = Results.begin(), End = Results.end();
       i != Results.end(); ++i) {
    i->second->writeEquation(DI, OS);
  }
}

AnalysisResult *ValueInfo::getOrCreateAlgorithmResult(const AlgorithmId *Id,
    AlgorithmFn Fn) {
  AnalysisResult *&AR = Results[Id];
  if (!AR) {
    AR = (*Fn)(this);
    assert(AR);
  }
  return AR;
}

AnalysisResult *ValueInfo::getAlgorithmResultOrNull(const AlgorithmId *Id)
    const {
  ResultsMapTy::const_iterator i = Results.find(Id);
  if (i == Results.end()) {
    return 0;
  }
  return i->second;
}

void ValueInfo::addInstructionAnalysisWorkInternal(const AlgorithmId *Id1,
    AlgorithmFn Fn1, ValueInfo *that, const AlgorithmId *Id2,
    AlgorithmFn Fn2) {
  getOrCreateAlgorithmResult(Id1, Fn1)->appendSubset(
      that->getOrCreateAlgorithmResult(Id2, Fn2));
}

}
}
