//===- EnumerationState.h -------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#ifndef ENUMERATIONSTATE_H
#define ENUMERATIONSTATE_H

#include <cstddef>
#include <deque>
#include <set>
#include <vector>

namespace llvm {

class raw_ostream;

}

namespace llvm {
namespace andersen_internal {

class AnalysisResult;
class DebugInfo;
class EnumerateElementsResult;
class ValueInfo;

class EnumerationState {
  typedef std::vector<ValueInfo *> ValueInfoVector;

  class ScopedEnumerateContext;
  
  struct ContentEnumerator {
    explicit ContentEnumerator(AnalysisResult *AR);

    AnalysisResult *const AR;
    size_t j;
  };

  int EnumerationDepth;
  unsigned int ExitCount;
  ValueInfoVector Elements;
  std::set<AnalysisResult *> Subsets;
  std::deque<ContentEnumerator> ContentEnumerators;

public:
  EnumerationState(AnalysisResult *AR);

  EnumerateElementsResult enumerateElements(size_t i, int Depth);

  void writeEquation(const DebugInfo &DI, raw_ostream &OS) const;

  const std::vector<ValueInfo *> &getElementsSoFar() const { return Elements; }

  bool isDone() const { return ContentEnumerators.empty(); }

private:
  void addSubset(AnalysisResult *AR);

  bool isEnumerating() const { return EnumerationDepth >= 0; }
};

}
}

#endif
