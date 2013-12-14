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

#include <cstddef>

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class EnumerationResult;
  class GraphEdge;

  class Enumerator {
    AnalysisResult *const AR;
    size_t i;

  public:
    explicit Enumerator(AnalysisResult *AR, size_t i = 0);

    EnumerationResult enumerate(int Depth, int LastTransformDepth);

    GraphEdge toGraphEdge() const;
  };
}
}

#endif
