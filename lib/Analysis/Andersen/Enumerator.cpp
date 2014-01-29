//===- llvm/Analysis/Enumerator.cpp - internal enumerator class -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the internal Enumerator class.
//
//===----------------------------------------------------------------------===//

#include "Enumerator.h"

#include "AnalysisResult.h"
#include "DebugInfo.h"
#include "EnumerationResult.h"
#include "GraphNode.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>

namespace llvm {
namespace andersen_internal {

EnumerationResult Enumerator::enumerate(int Depth, int LastTransformDepth) {
  return AR->enumerate(Depth, LastTransformDepth, i);
}

GraphEdge Enumerator::toGraphEdge() const {
  std::ostringstream OSS;
  OSS << "Index " << i;
  return GraphEdge(AR, OSS.str());
}

void Enumerator::writeFormula(const DebugInfo &DI, raw_ostream &OS) const {
  DI.printAnalysisResultName(AR, OS);
  OS << '[' << i << ":]";
}

}
}
