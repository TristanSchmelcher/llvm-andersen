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

#include "LazyAndersenAlgorithmTraits.h"
#include "LazyAndersenAnalysisResult.h"

namespace llvm {
namespace lazyandersen {
  template<typename AlgorithmIdTy>
  AlgorithmResultCache<AlgorithmIdTy>::AlgorithmResultCache() {}

  template<typename AlgorithmIdTy>
  AlgorithmResultCache<AlgorithmIdTy>::~AlgorithmResultCache() {}

  template<typename AlgorithmIdTy>
  template<AlgorithmIdTy AlgorithmId>
  AnalysisResult *
  AlgorithmResultCache<AlgorithmIdTy>::getAlgorithmResult() {
    return getAlgorithmResultInternal(AlgorithmId,
        typename AlgorithmGroupTraits<AlgorithmIdTy>
            ::template AlgorithmTraits<AlgorithmId>::Type(),
        static_cast<InputType *>(this));
  }

  template<typename AlgorithmIdTy>
  AnalysisResult *
  AlgorithmResultCache<AlgorithmIdTy>::getAlgorithmResultInternal(
      AlgorithmIdTy AlgorithmId,
      const AnalysisAlgorithm<InputType> &Algorithm, InputType *Input) {
    AnalysisResult *Result = Results[AlgorithmId].get();
    if (!Result) {
      Result = new AnalysisResult();
      Algorithm.getLazyResult(Result, Input);
      Results[AlgorithmId].reset(Result);
    }
    return Result;
  }
}
}

#endif
