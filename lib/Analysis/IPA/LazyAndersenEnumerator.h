//===- LazyAndersenEnumerator.h - algorithm classes -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for enumerating an analysis result's contents.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENENUMERATOR_H
#define LAZYANDERSENENUMERATOR_H

#include "LazyAndersenEnumerationResult.h"
#include "LazyAndersenGraphNode.h"

#include <cassert>

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  class Enumerator {
    AnalysisResult *const AR;
    size_t i;

  public:
    explicit Enumerator(AnalysisResult *AR) : AR(AR), i(0) {
      assert(AR);
    }

    EnumerationResult enumerate(int Depth);

    GraphEdge toGraphEdge() const;
  };
}
}

#endif
