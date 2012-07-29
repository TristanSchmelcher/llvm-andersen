//===- LazyAndersenAlgorithmResultCache.h - algorithm classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a cache for algorithm results for a particular group.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENALGORITHMRESULTCACHE_H
#define LAZYANDERSENALGORITHMRESULTCACHE_H

#include "llvm/ADT/OwningPtr.h"

namespace llvm {
namespace lazyandersen {
  template<typename InputType> class AnalysisAlgorithm;
  class AnalysisResult;

  typedef OwningPtr<AnalysisResult> AnalysisResultRef;

  template<typename AlgorithmIdTy>
  struct AlgorithmGroupTraits;

  template<typename AlgorithmIdTy>
  class AlgorithmResultCache : private AlgorithmGroupTraits<AlgorithmIdTy> {
    using AlgorithmGroupTraits<AlgorithmIdTy>::NumAlgorithmIds;
    typedef typename AlgorithmGroupTraits<AlgorithmIdTy>::InputType InputType;

    AnalysisResultRef Results[NumAlgorithmIds];

  public:
    AlgorithmResultCache();
    ~AlgorithmResultCache();

    template<AlgorithmIdTy AlgorithmId>
    AnalysisResult *getAlgorithmResult();

  private:
    AnalysisResult *getAlgorithmResultInternal(AlgorithmIdTy AlgorithmId,
        const AnalysisAlgorithm<InputType> &Algorithm, InputType *Input);
  };
}
}

#endif
