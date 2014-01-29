//===- llvm/Analysis/Enumerator.h - internal enumerator class -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the internal Enumerator class.
//
//===----------------------------------------------------------------------===//

#ifndef ENUMERATOR_H
#define ENUMERATOR_H

#include <cstddef>

namespace llvm {

class raw_ostream;

}

namespace llvm {
namespace andersen_internal {

class AnalysisResult;
class DebugInfo;
class EnumerationResult;
class GraphEdge;

class Enumerator {
  AnalysisResult *AR;
  size_t i;

public:
  explicit Enumerator(AnalysisResult *AR, size_t i = 0) : AR(AR), i(i) {}

  EnumerationResult enumerate(int Depth, int LastTransformDepth);
  GraphEdge toGraphEdge() const;
  void writeFormula(const DebugInfo &DI, raw_ostream &OS) const;

  AnalysisResult *getAnalysisResult() const { return AR; }

  size_t getPosition() const { return i; }
};

}
}

#endif
