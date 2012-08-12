//===- LazyAndersenReturnValueReversePointsToAlgorithm.h ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the return value reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRETURNVALUEREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENRETURNVALUEREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisResultAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  DECLARE_ALGORITHM(AnalysisResultAlgorithmId,
      RETURN_VALUE_REVERSE_POINTS_TO_SET);
}
}

#endif
