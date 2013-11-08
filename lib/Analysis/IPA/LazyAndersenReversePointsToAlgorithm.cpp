//===- LazyAndersenReversePointsToAlgorithm.cpp - reverse points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char ReversePointsToAlgorithm::ID[] = "reverse points-to";

AnalysisResult *ReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addValueInfo(VI);
  return AR;
}
