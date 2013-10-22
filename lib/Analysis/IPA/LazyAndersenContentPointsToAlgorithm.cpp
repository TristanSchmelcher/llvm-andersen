//===- LazyAndersenContentPointsToAlgorithm.cpp - content points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the content points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenValueInfoAlgorithmId.h"

#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ContentPointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ContentPointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<CONTENT_POINTS_TO_SET_STEP2>();
    }

    virtual std::string getNodeLabel() const { return "ContentStep"; }
  };

  class ContentPointsToAnalysisStep2 : public MetaAnalysisStep {
  public:
    explicit ContentPointsToAnalysisStep2(AnalysisResult *Input)
      : MetaAnalysisStep(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<CONTENT_POINTS_TO_SET_STEP3>();
    }

    virtual std::string getNodeLabel() const { return "ContentStep2"; }
  };

  class ContentPointsToAnalysisStep3
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ContentPointsToAnalysisStep3(ValueInfo *Input)
      : RelationsAnalysisStep<DESTINATION>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeStoredValuesPointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ContentStep3"; }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_POINTS_TO_SET>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ContentPointsToAnalysisStep(
        Input->getAlgorithmResult<POINTS_TO_SET>()));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_POINTS_TO_SET_STEP2>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ContentPointsToAnalysisStep2(
        Input->getAlgorithmResult<REVERSE_POINTS_TO_SET>()));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_POINTS_TO_SET_STEP3>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ContentPointsToAnalysisStep3(Input));
    return Output;
  }
}
}
