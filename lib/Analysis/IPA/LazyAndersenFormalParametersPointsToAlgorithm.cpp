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
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class FormalParametersPointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit FormalParametersPointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<
          ActualParametersPointsToAlgorithm, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ArgumentStep"; }
  };
}

const char FormalParametersPointsToAlgorithm::ID[] =
    "formal params points-to";

AnalysisResult *FormalParametersPointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new FormalParametersPointsToAnalysisStep(
      VI->getAlgorithmResult<ReversePointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
