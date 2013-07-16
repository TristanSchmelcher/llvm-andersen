//===- LazyAndersenEnumerator.h - lazy set enumerator ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares an enumerator for lazy sets.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENENUMERATOR_H
#define LAZYANDERSENENUMERATOR_H

#include "llvm/ADT/DenseSet.h"

namespace llvm {
  class Value;

namespace lazyandersen {
  class AnalysisResult;

  // TODO: Rewrite as an lazy iterator.
  class Enumerator {
  public:
    static void enumerate(AnalysisResult *AR, DenseSet<const Value *> *Out);
  };
}
}

#endif
