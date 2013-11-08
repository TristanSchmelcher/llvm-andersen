//===- LazyAndersenContentReversePointsToAlgorithm.cpp --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the content reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenContentReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenIsNotNecessarilyEmptyIfMissingProperty.h"
#include "LazyAndersenLoadedValuesReversePointsToAlgorithm.h"
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ContentReversePointsToAnalysisStep2 : public MetaAnalysisStep {
  public:
    explicit ContentReversePointsToAnalysisStep2(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<
          LoadedValuesReversePointsToAlgorithm, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ContentReverseStep2"; }
  };

  struct ContentReversePointsToAlgorithmStep2 :
      public IsNotNecessarilyEmptyIfMissingProperty {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };

  const char ContentReversePointsToAlgorithmStep2::ID[] =
      "content reverse points-to 2";

  AnalysisResult *ContentReversePointsToAlgorithmStep2::run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->addWork(new ContentReversePointsToAnalysisStep2(
        VI->getAlgorithmResult<ReversePointsToAlgorithm,
            ENUMERATION_PHASE>()));
    return AR;
  }

  class ContentReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ContentReversePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ContentReversePointsToAlgorithmStep2,
          ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ContentReverseStep"; }
  };
}

const char ContentReversePointsToAlgorithm::ID[] = "content reverse points-to";

AnalysisResult *ContentReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ContentReversePointsToAnalysisStep(
      VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
