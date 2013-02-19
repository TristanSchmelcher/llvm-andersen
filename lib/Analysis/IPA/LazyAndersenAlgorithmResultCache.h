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
  template<typename AlgorithmIdTy>
  struct AlgorithmGroup;

  template<typename AlgorithmIdTy>
  class AlgorithmResultCache : private AlgorithmGroup<AlgorithmIdTy> {
    using AlgorithmGroup<AlgorithmIdTy>::NumAlgorithmIds;
    typedef typename AlgorithmGroup<AlgorithmIdTy>::InputTy InputTy;
    typedef typename AlgorithmGroup<AlgorithmIdTy>::OutputTy OutputTy;
    typedef typename AlgorithmGroup<AlgorithmIdTy>::AlgorithmTy AlgorithmTy;

    OwningPtr<OutputTy> Results[NumAlgorithmIds];

  public:
    AlgorithmResultCache();
    ~AlgorithmResultCache();

    template<AlgorithmIdTy AlgorithmId>
    OutputTy *getAlgorithmResult();

  private:
    OutputTy *getAlgorithmResultInternal(AlgorithmIdTy AlgorithmId,
        AlgorithmTy Algorithm, InputTy *Input);
  };
}
}

#endif
