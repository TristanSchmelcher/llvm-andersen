//===- LazyAndersenFormalParametersPointsToAlgorithm.cpp ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the formal parameters points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenFormalParametersPointsToAlgorithm.h"

#include "LazyAndersenActualParametersPointsToAlgorithm.h"
#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenTransformStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char FormalParametersPointsToAlgorithm::ID[] = "formal params";

AnalysisResult *FormalParametersPointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new TransformStep<ActualParametersPointsToAlgorithm>(
      VI->getAlgorithmResult<ReversePointsToAlgorithm,
          INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
