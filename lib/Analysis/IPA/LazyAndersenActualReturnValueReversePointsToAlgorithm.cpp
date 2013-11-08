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
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ActualReturnValueReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ActualReturnValueReversePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<
          FormalReturnValueReversePointsToAlgorithm, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const {
      return "ReturnValueReverseStep";
    }
  };
}

const char ActualReturnValueReversePointsToAlgorithm::ID[] =
    "actual return value reverse points-to";

AnalysisResult *ActualReturnValueReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ActualReturnValueReversePointsToAnalysisStep(
      VI->getAlgorithmResult<ReversePointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
