//===- EnumerationContext.h - algorithm classes ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for the context of an analysis result enumeration.
//
//===----------------------------------------------------------------------===//

#ifndef ENUMERATIONCONTEXT_H
#define ENUMERATIONCONTEXT_H

#include "AnalysisResult.h"
#include "AnalysisResultWork.h"
#include "RecursiveEnumerate.h"

namespace llvm {
namespace andersen_internal {

class RecursiveEnumerate;

class ScopedSetEnumerating {
  friend class EnumerationContext;

  AnalysisResult *const AR;

  ScopedSetEnumerating(AnalysisResult *AR, int Depth);
  ~ScopedSetEnumerating();
};

class EnumerationContext : private ScopedSetEnumerating {
  friend class Enumerator;

  const int Depth;
  const int LastTransformDepth;
  AnalysisResultWorkList::iterator Pos;

  EnumerationContext(AnalysisResult *AR, int Depth, int LastTransformDepth);
  ~EnumerationContext();

public:
  int getDepth() const { return Depth; }

  int getLastTransformDepth() const { return LastTransformDepth; }

  int getNextDepth() const { return getDepth() + 1; }

  AnalysisResult *getAnalysisResult() const { return AR; }

  bool canInline() const {
    assert(!AR->isDone());
    // If this AR has a work list containing a sole RecursiveEnumerate and the
    // level above is also a RecursiveEnumerate, then the inner one can be
    // inlined into the upper one. Doing so gives up the chance to share the
    // work done to filter out repeated VIs, but it has the advantage that
    // redundant ARs can be erased, which is necessary when retries are
    // involved.
    return getDepth() > getLastTransformDepth() + 1 &&
           ++AR->Work.begin() == AR->Work.end();
  }

  RecursiveEnumerate *pushSubset(AnalysisResult *Subset) {
    if (AR->prepareForSubset(Subset)) {
      RecursiveEnumerate *RE = new RecursiveEnumerate(Subset);
      Pos = AR->Work.insert(Pos, RE);
      return RE;
    } else {
      return 0;
    }
  }
};

}
}

#endif
