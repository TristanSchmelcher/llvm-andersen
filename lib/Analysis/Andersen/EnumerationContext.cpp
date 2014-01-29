//===- EnumerationContext.cpp - algorithm classes -------------------------===//
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

#include "EnumerationContext.h"

#include "AnalysisResult.h"
#include "EnumerationResult.h"
#include "RecursiveEnumerate.h"

#include <cassert>

namespace llvm {
namespace andersen_internal {

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

EnumerationContext::EnumerationContext(AnalysisResult *AR, int Depth,
    int LastTransformDepth)
  : ScopedSetEnumerating(AR, Depth),
    Depth(Depth),
    LastTransformDepth(LastTransformDepth),
    Pos(AR->Work.begin()) {}

EnumerationContext::~EnumerationContext() {}

}
}
