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

#include "llvm/Analysis/AndersenEnumerator.h"

namespace llvm {

class raw_ostream;

}

namespace llvm {
namespace andersen_internal {

class Data;
class EnumerationResult;
class GraphEdge;

class Enumerator : private EnumeratorState {
  // Must not have any fields.
public:
  explicit Enumerator(AnalysisResult *AR, size_t i = 0)
    : EnumeratorState(AR, i) {}

  static Enumerator &forState(EnumeratorState &ES) {
    // Since Enumerator has no data other than what it inherits from
    // EnumeratorState, the downcast is safe. This trick lets us avoid declaring
    // the internal Enumerator methods in a public header.
    return static_cast<Enumerator &>(ES);
  }

  EnumerationResult enumerate(int Depth, int LastTransformDepth);
  GraphEdge toGraphEdge() const;
  void writeFormula(const Data &Data, raw_ostream &OS) const;

  AnalysisResult *getAnalysisResult() const { return AR; }

  size_t getPosition() const { return i; }
};

}
}

#endif
