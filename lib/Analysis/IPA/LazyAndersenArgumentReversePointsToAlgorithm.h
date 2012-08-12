//===- LazyAndersenArgumentReversePointsToAlgorithm.h ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the argument reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENARGUMENTREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENARGUMENTREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisResultAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  DECLARE_ALGORITHM(AnalysisResultAlgorithmId, ARGUMENT_REVERSE_POINTS_TO_SET);
}
}

#endif
