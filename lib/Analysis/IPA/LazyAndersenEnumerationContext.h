//===- LazyAndersenEnumerationContext.h - algorithm classes ---------------===//
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

#ifndef LAZYANDERSENENUMERATIONCONTEXT_H
#define LAZYANDERSENENUMERATIONCONTEXT_H

#include "LazyAndersenAnalysisResultWork.h"
#include "LazyAndersenEnumerationResult.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  class ScopedSetEnumerating {
    friend class EnumerationContext;

    AnalysisResult *const AR;

    ScopedSetEnumerating(AnalysisResult *AR, int Depth);
    ~ScopedSetEnumerating();
  };

  class EnumerationContext : private ScopedSetEnumerating {
    friend class Enumerator;

    const int NextDepth;
    AnalysisResultWorkList::iterator Pos;

    EnumerationContext(AnalysisResult *AR, int Depth);
    ~EnumerationContext();

  public:
    int getNextDepth() const { return NextDepth; }

    EnumerationResult pushWork(AnalysisResult *Child);
  };
}
}

#endif
