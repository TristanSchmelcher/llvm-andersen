//===- LazyAndersenReturnValuePointsToAlgorithm.cpp - return val points-to ===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the return value points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReturnValuePointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::lazyandersen;

DEFINE_ALGORITHM(AnalysisResultAlgorithmId, RETURN_VALUE_POINTS_TO_SET, Input) {
  // TODO
  return 0;
}
