//===- AnalysisResult.cpp - algorithm classes -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for analysis results.
//
//===----------------------------------------------------------------------===//

#include "AnalysisResult.h"

#include "AlgorithmId.h"
#include "ValueInfo.h"

#include <sstream>

namespace llvm {
namespace andersen_internal {

inline std::string AnalysisResultId::buildNodeLabel(const Data &Data,
    const AnalysisResult *Owner) const {
  std::ostringstream OSS;
#ifndef NDEBUG
  if (!Input) {
    // This is the special empty AR.
    assert(!AlgoId);
    OSS << "EmptySet";
  } else {
    assert(AlgoId);
    OSS << AlgoId->getAlgorithmName() << '(' << Input->getNodeLabel(Data)
        << ')';
  }
#else
  // Use the address of the AR as a unique id.
  OSS << Owner;
#endif
  return OSS.str();
}

AnalysisResult::AnalysisResult(AnalysisResultId Id)
  : EnumerationDepth(-1), Id(Id) {}

AnalysisResult::~AnalysisResult() {
  assert(!isEnumerating());
}

GraphEdgeDeque AnalysisResult::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  size_t Pos = 0;
  for (ValueInfoSetVector::const_iterator i = Set.begin(), End = Set.end();
       i != End; ++i, ++Pos) {
    std::ostringstream OSS;
    OSS << Pos;
    Result.push_back(GraphEdge(*i, OSS.str()));
  }
  for (AnalysisResultWorkList::const_iterator i = Work.begin(),
                                              End = Work.end();
       i != End; ++i, ++Pos) {
    const AnalysisResultWork *Ent = &*i;
    std::ostringstream OSS;
    OSS << Pos;
    Result.push_back(GraphEdge(Ent, OSS.str()));
  }
  return Result;
}

std::string AnalysisResult::getNodeLabel(const Data &Data) const {
  return Id.buildNodeLabel(Data, this);
}

bool AnalysisResult::isNodeHidden() const {
  return false;
}

}
}
