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

#include "DebugInfo.h"
#include "ValueInfo.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <sstream>

namespace llvm {
namespace andersen_internal {

AnalysisResult::AnalysisResult() : EnumerationDepth(-1) {}

AnalysisResult::~AnalysisResult() {
  assert(!isEnumerating());
}

void AnalysisResult::writeEquation(const DebugInfo &DI, raw_ostream &OS) const {
  DI.printAnalysisResultName(this, OS);
  OS << " = ";
  bool first = true;
  if (!Set.empty()) {
    OS << '{';
    ValueInfoSetVector::const_iterator i = Set.begin(), End = Set.end();
    do {
      if (!first) {
        OS << ", ";
      }
      first = false;
      DI.printValueInfoName(*i, OS);
    } while (++i != End);
    OS << '}';
  }
  for (AnalysisResultWorkList::const_iterator i = Work.begin(),
                                              End = Work.end();
       i != End; ++i) {
    if (!first) {
      OS << " U ";
    }
    first = false;
    i->writeFormula(DI, OS);
  }
  if (first) {
    // Empty.
    OS << "{}";
  }
  OS << '\n';
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

void AnalysisResult::printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const {
  DI.printAnalysisResultName(this, OS);
}

bool AnalysisResult::isNodeHidden() const {
  return false;
}

}
}
