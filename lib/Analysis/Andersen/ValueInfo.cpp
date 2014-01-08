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
#include "Data.h"
#include "RecursiveEnumerate.h"
#include "ValuePrinter.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>

namespace llvm {
namespace andersen_internal {

ValueInfo *const ValueInfo::Nil = 0;

ValueInfo::ValueInfo(const Value *V) : V(V) {}

ValueInfo::~ValueInfo() {
  DeleteContainerSeconds(Results);
}

GraphEdgeDeque ValueInfo::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  for (ResultsMapTy::const_iterator i = Results.begin(); i != Results.end();
       ++i) {
    const AlgorithmId *Id = i->first;
    AnalysisResult *AR = i->second;
    std::ostringstream OSS;
    OSS << "PointsTo(" << Id->getAlgorithmName() << ')';
    Result.push_back(GraphEdge(AR, OSS.str()));
  }
  return Result;
}

std::string ValueInfo::getNodeLabel(const Data &Data) const {
  if (getValue()) {
    static const size_t MaxPrintedSize = 16;
    return ValuePrinter::prettyPrintValue(getValue(), MaxPrintedSize);
  } else if (this == Data.ExternallyLinkableRegions.getPtr()) {
    return "ExternallyLinkableRegions";
  } else if (this == Data.ExternallyAccessibleRegions.getPtr()) {
    return "ExternallyAccessibleRegions";
  } else {
    std::ostringstream OSS;
    OSS << "Anonymous" << this;
    return OSS.str();
  }
}

bool ValueInfo::isNodeHidden() const {
  return false;
}

void ValueInfo::writeEquations(const Data &Data, raw_ostream &OS) const {
  for (ResultsMapTy::const_iterator i = Results.begin(); i != Results.end();
       ++i) {
    i->second->writeEquation(Data, OS);
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
  getOrCreateAlgorithmResult(Id1, Fn1)->addWork(
      new RecursiveEnumerate(that->getOrCreateAlgorithmResult(Id2, Fn2)));
}

}
}
