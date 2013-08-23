//===- LazyAndersenAnalysisResult.cpp - algorithm classes -----------------===//
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

#include "LazyAndersenAnalysisResult-inl.h"

#include "LazyAndersenAnalysisResultEntryBase.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisResult::AnalysisResult() : Enumerating(false) {}

AnalysisResult::~AnalysisResult() {}

GraphEdgeDeque AnalysisResult::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  for (AnalysisResultEntryBaseList::const_iterator i = begin(), End = end();
       i != End; ++i) {
    const AnalysisResultEntryBase *Ent = &*i;
    std::ostringstream OSS;
    OSS << Ent;
    Result.push_back(GraphEdge(Ent->getGraphNode(), OSS.str()));
  }
  for (int i = 0; i < NUM_ANALYSIS_RESULT_ALGORITHMS; ++i) {
    AnalysisResult *AR = ResultCache.getAlgorithmResultNoCreate(
        static_cast<AnalysisResultAlgorithmId>(i));
    if (!AR) continue;
    Result.push_back(GraphEdge(AR, AnalysisResultAlgorithmNames[i]));
  }
  return Result;
}

std::string AnalysisResult::getNodeLabel() const {
  return std::string();
}

bool AnalysisResult::isNodeHidden() const {
  return false;
}
