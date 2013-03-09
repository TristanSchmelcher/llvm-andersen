//===- LazyAndersenAnalysisResultAlgorithmId.h - algorithm classes --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares ids for AnalysisResult algorithms.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTALGORITHMID_H
#define LAZYANDERSENANALYSISRESULTALGORITHMID_H

#include "LazyAndersenAlgorithmGroup.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  enum AnalysisResultAlgorithmId {
    CONTENT_POINTS_TO_SET,
    CONTENT_REVERSE_POINTS_TO_SET,
    RETURN_VALUE_POINTS_TO_SET,
    RETURN_VALUE_REVERSE_POINTS_TO_SET,
    ARGUMENT_POINTS_TO_SET,
    ARGUMENT_REVERSE_POINTS_TO_SET,
    // TODO
    NUM_ANALYSIS_RESULT_ALGORITHMS
  };

  template<>
  struct AlgorithmGroupTraits<AnalysisResultAlgorithmId> {
    static const AnalysisResultAlgorithmId NumAlgorithms =
        NUM_ANALYSIS_RESULT_ALGORITHMS;
    typedef AnalysisResult InputTy;
    typedef AnalysisResult OutputTy;
  };

  // The standard requires each specialization to be forward-declared, although
  // in practice it isn't needed (C++11 14.7.3/6).
  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               CONTENT_POINTS_TO_SET>(AnalysisResult *);

  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               CONTENT_REVERSE_POINTS_TO_SET>(AnalysisResult *);

  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               RETURN_VALUE_POINTS_TO_SET>(AnalysisResult *);

  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               RETURN_VALUE_REVERSE_POINTS_TO_SET>(
      AnalysisResult *);

  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               ARGUMENT_POINTS_TO_SET>(AnalysisResult *);

  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               ARGUMENT_REVERSE_POINTS_TO_SET>(
      AnalysisResult *);
}
}

#endif
