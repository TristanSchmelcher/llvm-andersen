//===- LazyAndersenActualReturnValueReversePointsToAlgorithm.cpp ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the actual return value reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenActualReturnValueReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenFormalReturnValueReversePointsToAlgorithm.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenTransformStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char ActualReturnValueReversePointsToAlgorithm::ID[] =
    "actual return value";

AnalysisResult *ActualReturnValueReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new TransformStep<FormalReturnValueReversePointsToAlgorithm>(
      VI->getAlgorithmResult<ReversePointsToAlgorithm,
          INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
