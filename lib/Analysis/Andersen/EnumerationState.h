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
namespace andersen_internal {

class AnalysisResult;
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

  struct RetryEntry {
    RetryEntry(EnumerationState *ES);

    int EnumerationDepth;
    EnumerationState *ES;
    unsigned int ExitCount;
    ValueInfoVector::size_type ElementsSize;
  };

  // Orders by decreasing depth.
  struct RetryEntryComparator {
    bool operator()(const RetryEntry &E1, const RetryEntry &E2) const;
  };

  typedef std::set<RetryEntry, RetryEntryComparator> RetryState;

  int EnumerationDepth;
  unsigned int ExitCount;
  RetryState RS;
  ValueInfoVector Elements;
  std::set<AnalysisResult *> Subsets;
  std::deque<ContentEnumerator> ContentEnumerators;

public:
  EnumerationState(AnalysisResult *AR);

  EnumerateElementsResult enumerateElements(size_t i, int Depth);

  const std::vector<ValueInfo *> &getElementsSoFar() const { return Elements; }

  bool isDone() const { return ContentEnumerators.empty(); }

private:
  void addSubset(AnalysisResult *AR);

  bool haveCachedRetryResult();

  bool isEnumerating() const { return EnumerationDepth >= 0; }
};

}
}

#endif
