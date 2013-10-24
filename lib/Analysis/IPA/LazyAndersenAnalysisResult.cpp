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

#include "LazyAndersenAnalysisResult.h"

#include "LazyAndersenValueInfo.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisResult::AnalysisResult() : EnumerationDepth(-1) {}

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

std::string AnalysisResult::getNodeLabel() const {
  return std::string();
}

bool AnalysisResult::isNodeHidden() const {
  return false;
}
