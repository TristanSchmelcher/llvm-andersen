//===- LazyAndersenArgumentPointsToAlgorithm.h - argument points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the argument points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENARGUMENTPOINTSTOALGORITHM_H
#define LAZYANDERSENARGUMENTPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisResultAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  DECLARE_ALGORITHM(AnalysisResultAlgorithmId, ARGUMENT_POINTS_TO_SET);
}
}

#endif
