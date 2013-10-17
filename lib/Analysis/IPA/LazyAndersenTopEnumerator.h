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

#ifndef LAZYANDERSENTOPENUMERATOR_H
#define LAZYANDERSENTOPENUMERATOR_H

#include "LazyAndersenAnalysisResult.h"

namespace llvm {
namespace lazyandersen {
  class TopEnumerator {
    AnalysisResult::Enumerator E;

  public:
    explicit TopEnumerator(AnalysisResult *AR);
    ~TopEnumerator();

    // Get next VI or null.
    ValueInfo *enumerate();
  };
}
}

#endif
