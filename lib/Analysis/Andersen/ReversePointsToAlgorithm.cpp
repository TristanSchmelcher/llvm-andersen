//===- ReversePointsToAlgorithm.cpp - reverse points-to -------------------===//
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

#include "ReversePointsToAlgorithm.h"

#include "AnalysisResult.h"

namespace llvm {
namespace andersen_internal {

const LiteralAlgorithmId ReversePointsToAlgorithm::ID("self-reverse");

AnalysisResult *ReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult(AnalysisResultId(&ID, VI));
  AR->addValueInfo(VI);
  return AR;
}

}
}
