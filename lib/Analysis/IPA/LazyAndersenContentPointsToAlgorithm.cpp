//===- LazyAndersenContentPointsToAlgorithm.cpp - content points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the content points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenContentPointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::lazyandersen;

DEFINE_ALGORITHM(AnalysisResultAlgorithmId, CONTENT_POINTS_TO_SET, Input) {
  // TODO
  return 0;
}
