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
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ActualParametersReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ActualParametersReversePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<
          FormalParametersReversePointsToAlgorithm, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ArgumentReverseStep"; }
  };
}

const char ActualParametersReversePointsToAlgorithm::ID[] =
    "actual params reverse points-to";

AnalysisResult *ActualParametersReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ActualParametersReversePointsToAnalysisStep(
      VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
