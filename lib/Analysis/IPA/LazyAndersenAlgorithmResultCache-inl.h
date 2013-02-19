//===- LazyAndersenAlgorithmResultCache-inl.h - algorithm classes ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for a cache for algorithm results for a
// particular group.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENALGORITHMRESULTCACHE_INL_H
#define LAZYANDERSENALGORITHMRESULTCACHE_INL_H

#include "LazyAndersenAlgorithmResultCache.h"

namespace llvm {
namespace lazyandersen {
  template<typename AlgorithmIdTy>
  AlgorithmResultCache<AlgorithmIdTy>::AlgorithmResultCache() {}

  template<typename AlgorithmIdTy>
  AlgorithmResultCache<AlgorithmIdTy>::~AlgorithmResultCache() {}

  template<typename AlgorithmIdTy>
  template<AlgorithmIdTy AlgorithmId>
  typename AlgorithmResultCache<AlgorithmIdTy>::OutputTy *
  AlgorithmResultCache<AlgorithmIdTy>::getAlgorithmResult() {
    return getAlgorithmResultInternal(AlgorithmId,
        &AlgorithmGroup<AlgorithmIdTy>
            ::template AlgorithmImplementation<AlgorithmId>::run,
        static_cast<InputTy *>(this));
  }

  template<typename AlgorithmIdTy>
  typename AlgorithmResultCache<AlgorithmIdTy>::OutputTy *
  AlgorithmResultCache<AlgorithmIdTy>::getAlgorithmResultInternal(
      AlgorithmIdTy AlgorithmId, AlgorithmTy Algorithm,
      InputTy *Input) {
    OutputTy *Result = Results[AlgorithmId].get();
    if (!Result) {
      Result = (*Algorithm)(Input);
      Results[AlgorithmId].reset(Result);
    }
    return Result;
  }
}
}

#endif
