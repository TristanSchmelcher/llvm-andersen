//===- EnumerationContext.h - algorithm classes ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for the context of an analysis result enumeration.
//
//===----------------------------------------------------------------------===//

#ifndef ENUMERATIONCONTEXT_H
#define ENUMERATIONCONTEXT_H

#include "AnalysisResult.h"
#include "AnalysisResultWork.h"
#include "SubsetWork.h"

namespace llvm {
namespace andersen_internal {

class ScopedSetEnumerating {
  friend class EnumerationContext;

  AnalysisResult *const AR;

  ScopedSetEnumerating(AnalysisResult *AR, int Depth) : AR(AR) {
    assert(!AR->isEnumerating());
    assert(Depth >= 0);
    AR->EnumerationDepth = Depth;
  }

  ~ScopedSetEnumerating() {
    assert(AR->isEnumerating());
    AR->EnumerationDepth = -1;
  }
};

class EnumerationContext : private ScopedSetEnumerating {
  friend class AnalysisResult;

  const int Depth;
  const int LastTransformDepth;
  AnalysisResultWorkList::iterator Pos;

  EnumerationContext(AnalysisResult *AR, int Depth, int LastTransformDepth)
    : ScopedSetEnumerating(AR, Depth),
      Depth(Depth),
      LastTransformDepth(LastTransformDepth),
      Pos(AR->Work.begin()) {}

public:
  int getDepth() const { return Depth; }

  int getLastTransformDepth() const { return LastTransformDepth; }

  int getNextDepth() const { return getDepth() + 1; }

  AnalysisResult *getAnalysisResult() const { return AR; }

  bool canInline() const {
    assert(!AR->isDone());
    // If this AR has a work list containing a sole SubsetWork and the level
    // above is also a SubsetWork, then the inner one can be inlined into the
    // upper one. Doing so gives up the chance to share the work done to filter
    // out repeated VIs, but it has the advantage that redundant ARs can be
    // erased, which is necessary when retries are involved.
    return getDepth() > getLastTransformDepth() + 1 &&
           ++AR->Work.begin() == AR->Work.end();
  }

  SubsetWork *pushSubset(AnalysisResult *Subset) {
    if (AR->prepareForSubset(Subset)) {
      SubsetWork *SW = new SubsetWork(Subset);
      Pos = AR->Work.insert(Pos, SW);
      return SW;
    } else {
      return 0;
    }
  }

  void pop() {
    assert(Pos != AR->Work.end());
    Pos = AR->Work.erase(Pos);
  }
};

}
}

#endif
