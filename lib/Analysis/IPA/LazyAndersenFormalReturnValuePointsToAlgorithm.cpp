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
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class FormalReturnValuePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit FormalReturnValuePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<
          ActualReturnValuePointsToAlgorithm, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ReturnValueStep"; }
  };
}

const char FormalReturnValuePointsToAlgorithm::ID[] =
    "formal return value points-to";

AnalysisResult *FormalReturnValuePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new FormalReturnValuePointsToAnalysisStep(
      VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
