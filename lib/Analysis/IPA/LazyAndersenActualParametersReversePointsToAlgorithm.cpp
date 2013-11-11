//===- LazyAndersenActualParametersReversePointsToAlgorithm.cpp -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the actual parameters reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenActualParametersReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenFormalParametersReversePointsToAlgorithm.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenTransformStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char ActualParametersReversePointsToAlgorithm::ID[] =
    "actual params reverse";

AnalysisResult *ActualParametersReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new TransformStep<FormalParametersReversePointsToAlgorithm>(
      VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
