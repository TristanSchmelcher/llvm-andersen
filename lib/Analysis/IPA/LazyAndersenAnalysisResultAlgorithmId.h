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

#include "LazyAndersenAlgorithmTraits.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  enum AnalysisResultAlgorithmId {
    CONTENT_POINTS_TO_SET,
    RETURN_VALUE_POINTS_TO_SET,
    ARGUMENT_POINTS_TO_SET,
    // TODO
    NUM_LAZY_ANALYSIS_RESULT_ALGORITHM_IDS
  };

  ALGORITHM_GROUP_TRAITS(AnalysisResultAlgorithmId,
                         NUM_LAZY_ANALYSIS_RESULT_ALGORITHM_IDS,
                         AnalysisResult);
}
}

#endif
