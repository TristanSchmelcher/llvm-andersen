//===- LazyAndersenFormalReturnValuePointsToAlgorithm.cpp -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the formal return value points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenFormalReturnValuePointsToAlgorithm.h"

#include "LazyAndersenActualReturnValuePointsToAlgorithm.h"
#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenTransformStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char FormalReturnValuePointsToAlgorithm::ID[] = "formal return value";

AnalysisResult *FormalReturnValuePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new TransformStep<ActualReturnValuePointsToAlgorithm>(
      VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
