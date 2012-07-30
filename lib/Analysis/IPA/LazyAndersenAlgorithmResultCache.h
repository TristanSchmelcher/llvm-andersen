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
  template<typename InputTy, typename OutputTy> class AnalysisAlgorithm;

  template<typename AlgorithmIdTy>
  struct AlgorithmGroupTraits;

  template<typename AlgorithmIdTy>
  class AlgorithmResultCache : private AlgorithmGroupTraits<AlgorithmIdTy> {
    using AlgorithmGroupTraits<AlgorithmIdTy>::NumAlgorithmIds;
    typedef typename AlgorithmGroupTraits<AlgorithmIdTy>::InputTy InputTy;
    typedef typename AlgorithmGroupTraits<AlgorithmIdTy>::OutputTy OutputTy;
    typedef AnalysisAlgorithm<InputTy, OutputTy> AlgorithmBaseTy;

    OwningPtr<OutputTy> Results[NumAlgorithmIds];

  public:
    AlgorithmResultCache();
    ~AlgorithmResultCache();

    template<AlgorithmIdTy AlgorithmId>
    OutputTy *getAlgorithmResult();

  private:
    OutputTy *getAlgorithmResultInternal(AlgorithmIdTy AlgorithmId,
        const AlgorithmBaseTy &Algorithm, InputTy *Input);
  };
}
}

#endif
