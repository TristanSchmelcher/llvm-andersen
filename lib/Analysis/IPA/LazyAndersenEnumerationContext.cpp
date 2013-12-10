//===- LazyAndersenEnumerationContext.cpp - algorithm classes -------------===//
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

#include "LazyAndersenEnumerationContext.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenRecursiveEnumerate.h"

#include <cassert>

using namespace llvm;
using namespace llvm::lazyandersen;

inline ScopedSetEnumerating::ScopedSetEnumerating(AnalysisResult *AR, int Depth)
  : AR(AR) {
  assert(!AR->isEnumerating());
  assert(Depth >= 0);
  AR->EnumerationDepth = Depth;
}

inline ScopedSetEnumerating::~ScopedSetEnumerating() {
  assert(AR->isEnumerating());
  AR->EnumerationDepth = -1;
}

EnumerationContext::EnumerationContext(AnalysisResult *AR, int Depth)
  : ScopedSetEnumerating(AR, Depth),
    NextDepth(Depth + 1),
    Pos(AR->Work.begin()) {}

EnumerationContext::~EnumerationContext() {}

EnumerationResult EnumerationContext::pushWork(AnalysisResult *Child) {
  // If Child == AR, then it's superfluous, but in practice that doesn't happen.
  RecursiveEnumerate *RE = new RecursiveEnumerate(Child);
  Pos = AR->Work.insert(Pos, RE);
  return RE->enumerate(this);
}
