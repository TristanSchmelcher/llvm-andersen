//===- LazyAndersenAlgorithmResultCache.h - algorithm classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a cache for algorithm results for a particular group.
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
    template<AlgorithmIdTy AlgorithmId>
    OutputTy *getAlgorithmResult(InputTy *Input) {
      assert(AlgorithmId < NumAlgorithms);
      // Look up the algorithm function and dispatch to an internal method to do
      // the rest. The internal method will have the same compilation for all
      // template arguments, so the compiler can combine them into one
      // symbol.
      return getAlgorithmResultInternal(AlgorithmId,
          &runAlgorithm<AlgorithmIdTy, AlgorithmId>, Input);
    }

    void setAlgorithmResultSpecialCase(AlgorithmIdTy AlgorithmId,
        OutputTy *Output) {
      assert(!Results[AlgorithmId].get());
      Results[AlgorithmId].reset(Output);
    }

  private:
    OutputTy *getAlgorithmResultInternal(AlgorithmIdTy AlgorithmId,
        AlgorithmTy Algorithm, InputTy *Input) {
      OutputTy *Result = Results[AlgorithmId].get();
      if (!Result) {
        Result = (*Algorithm)(Input);
        Results[AlgorithmId].reset(Result);
      }
      return Result;
    }
  };
}
}

#endif
