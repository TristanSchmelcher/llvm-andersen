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

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/STLExtras.h"

#include <cassert>

namespace llvm {
namespace lazyandersen {
  template<typename InputTy, typename OutputTy, typename AlgorithmIdTy>
  class AlgorithmResultCache {
    typedef OutputTy *(*AlgorithmFn)(InputTy *);
  public:
    typedef DenseMap<AlgorithmIdTy, OutputTy *> ResultsMapTy;
  private:
    ResultsMapTy Results;

  public:
    ~AlgorithmResultCache() {
      DeleteContainerSeconds(Results);
    }

    const ResultsMapTy &getResults() const { return Results; }

    template<typename AlgorithmTy>
    OutputTy *getAlgorithmResult() {
      // Dispatch to a non-template method for less code size.
      return getAlgorithmResultInternal(AlgorithmTy::ID, &AlgorithmTy::run);
    }

    template<typename AlgorithmTy>
    void preCreateSpecialCaseResult(OutputTy *Output) {
      OutputTy *&Result = Results[AlgorithmTy::ID];
      assert(!Result);
      Result = Output;
      assert(Result);
    }

  private:
    OutputTy *getAlgorithmResultInternal(AlgorithmIdTy Id, AlgorithmFn Fn);
  };

  template<typename InputTy, typename OutputTy, typename AlgorithmIdTy>
  OutputTy *AlgorithmResultCache<InputTy, OutputTy, AlgorithmIdTy>
      ::getAlgorithmResultInternal(AlgorithmIdTy Id, AlgorithmFn Fn) {
    OutputTy *&Result = Results[Id];
    if (!Result) {
      Result = (*Fn)(static_cast<InputTy *>(this));
      assert(Result);
    }
    return Result;
  }
}
}

#endif
