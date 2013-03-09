//===- LazyAndersenAlgorithmResultCache.cpp - algorithm classes -----------===//
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

#include "LazyAndersenAlgorithmResultCache.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultAlgorithmId.h"
#include "LazyAndersenValueInfoAlgorithmId.h"

using namespace llvm;
using namespace llvm::lazyandersen;

template<typename AlgorithmIdTy>
AlgorithmResultCache<AlgorithmIdTy>::AlgorithmResultCache() {}

template<typename AlgorithmIdTy>
AlgorithmResultCache<AlgorithmIdTy>::~AlgorithmResultCache() {}

template<typename AlgorithmIdTy>
typename AlgorithmResultCache<AlgorithmIdTy>::OutputTy *
AlgorithmResultCache<AlgorithmIdTy>::getAlgorithmResultInternal(
    AlgorithmIdTy AlgorithmId, AlgorithmTy Algorithm, InputTy *Input) {
  OutputTy *Result = Results[AlgorithmId].get();
  if (!Result) {
    Result = (*Algorithm)(Input);
    Results[AlgorithmId].reset(Result);
  }
  return Result;
}

template class AlgorithmResultCache<AnalysisResultAlgorithmId>;
template class AlgorithmResultCache<ValueInfoAlgorithmId>;
