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

#include "LazyAndersenAlgorithmGroup.h"
#include "llvm/ADT/OwningPtr.h"

#include <cassert>

namespace llvm {
namespace lazyandersen {
  template<typename AlgorithmIdTy>
  class AlgorithmResultCache : private AlgorithmGroupTraits<AlgorithmIdTy> {
    using AlgorithmGroupTraits<AlgorithmIdTy>::NumAlgorithms;
    typedef typename AlgorithmGroupTraits<AlgorithmIdTy>::InputTy InputTy;
    typedef typename AlgorithmGroupTraits<AlgorithmIdTy>::OutputTy OutputTy;
    typedef OutputTy *(*AlgorithmTy)(InputTy *);

    OwningPtr<OutputTy> Results[NumAlgorithms];

  public:
    AlgorithmResultCache();
    ~AlgorithmResultCache();

    template<AlgorithmIdTy AlgorithmId>
    OutputTy *getAlgorithmResult() {
      assert(AlgorithmId < NumAlgorithms);
      return getAlgorithmResultInternal(AlgorithmId,
          &runAlgorithm<AlgorithmIdTy, AlgorithmId>,
          static_cast<InputTy *>(this));
    }

  private:
    OutputTy *getAlgorithmResultInternal(AlgorithmIdTy AlgorithmId,
        AlgorithmTy Algorithm, InputTy *Input);
  };
}
}

#endif
