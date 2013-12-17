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

#include "AnalysisResultWork.h"

namespace llvm {
namespace andersen_internal {

class AnalysisResult;

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

  EnumerationResult pushWork(AnalysisResult *Child);
};

}
}

#endif
