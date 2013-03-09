//===- LazyAndersenArgumentReversePointsToAlgorithm.cpp -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the argument reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenAnalysisResultAlgorithmId.h"

namespace llvm {
namespace lazyandersen {

  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               ARGUMENT_REVERSE_POINTS_TO_SET>(
      AnalysisResult *Input) {
    // TODO
    return 0;
  }

}
}
