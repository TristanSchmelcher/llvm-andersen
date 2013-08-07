//===- LazyAndersenValueInfoAlgorithmId.h - algorithm classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares ids for ValueInfo algorithms.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENVALUEINFOALGORITHMID_H
#define LAZYANDERSENVALUEINFOALGORITHMID_H

#include "LazyAndersenAlgorithmGroup.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;
  class AnalysisResult;

  enum ValueInfoAlgorithmId {
    POINTS_TO_SET,
    REVERSE_POINTS_TO_SET,
    CONTENT_POINTS_TO_SET_STEP3,
    CONTENT_REVERSE_POINTS_TO_SET_STEP3,
    ARGUMENT_POINTS_TO_SET_STEP2,
    ARGUMENT_REVERSE_POINTS_TO_SET_STEP2,
    RETURN_VALUE_POINTS_TO_SET_STEP2,
    RETURN_VALUE_REVERSE_POINTS_TO_SET_STEP2,
    NUM_VALUE_INFO_ALGORITHMS
  };

  extern const char *const ValueInfoAlgorithmNames[NUM_VALUE_INFO_ALGORITHMS];

  template<>
  struct AlgorithmGroupTraits<ValueInfoAlgorithmId> {
    static const ValueInfoAlgorithmId NumAlgorithms =
        NUM_VALUE_INFO_ALGORITHMS;
    typedef ValueInfo InputTy;
    typedef AnalysisResult OutputTy;
  };

  // The standard requires each specialization to be forward-declared, although
  // in practice it isn't needed (C++11 14.7.3/6).
  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId, POINTS_TO_SET>(
      ValueInfo *);

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId, REVERSE_POINTS_TO_SET>(
      ValueInfo *);

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_POINTS_TO_SET_STEP3>(
      ValueInfo *);

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_REVERSE_POINTS_TO_SET_STEP3>(
      ValueInfo *);

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               ARGUMENT_POINTS_TO_SET_STEP2>(
      ValueInfo *);

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               ARGUMENT_REVERSE_POINTS_TO_SET_STEP2>(
      ValueInfo *);

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               RETURN_VALUE_POINTS_TO_SET_STEP2>(
      ValueInfo *);

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               RETURN_VALUE_REVERSE_POINTS_TO_SET_STEP2>(
      ValueInfo *);
}
}

#endif
