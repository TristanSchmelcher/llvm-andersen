//===- LazyAndersenReturnValueReversePointsToAlgorithm.cpp ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the return value reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReturnValueReversePointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::lazyandersen;

DEFINE_ALGORITHM(AnalysisResultAlgorithmId, RETURN_VALUE_REVERSE_POINTS_TO_SET,
    Input) {
  // TODO
  return 0;
}
