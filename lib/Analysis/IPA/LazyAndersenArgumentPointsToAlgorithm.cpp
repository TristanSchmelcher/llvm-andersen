//===- LazyAndersenArgumentPointsToAlgorithm.cpp - argument points-to -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the argument points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenArgumentPointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::lazyandersen;

DEFINE_ALGORITHM(AnalysisResultAlgorithmId, ARGUMENT_POINTS_TO_SET, Input) {
  // TODO
  return 0;
}
