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

#include "LazyAndersenAnalysisResultAlgorithmId.h"

#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ContentPointsToAnalysisStep
    : public MetaAnalysisStep<CONTENT_POINTS_TO_SET> {
  public:
    explicit ContentPointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep<CONTENT_POINTS_TO_SET>(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<REVERSE_POINTS_TO_SET>()
          ->getAlgorithmResult<CONTENT_POINTS_TO_SET_STEP2>();
    }

    virtual const char *getWorkName() const { return "ContentStep"; }
  };

  class ContentPointsToAnalysisStep2
    : public MetaAnalysisStep<CONTENT_POINTS_TO_SET_STEP2> {
  public:
    explicit ContentPointsToAnalysisStep2(AnalysisResult *Input)
      : MetaAnalysisStep<CONTENT_POINTS_TO_SET_STEP2>(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<CONTENT_POINTS_TO_SET_STEP3>();
    }

    virtual const char *getWorkName() const { return "ContentStep2"; }
  };

  class ContentPointsToAnalysisStep3
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ContentPointsToAnalysisStep3(ValueInfo *Input)
      : RelationsAnalysisStep<DESTINATION>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeStoredValuesPointsToSet();
    }

    virtual const char *getWorkName() const { return "ContentStep3"; }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               CONTENT_POINTS_TO_SET>(
      AnalysisResult *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ContentPointsToAnalysisStep(Input));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               CONTENT_POINTS_TO_SET_STEP2>(
      AnalysisResult *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ContentPointsToAnalysisStep2(Input));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_POINTS_TO_SET_STEP3>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ContentPointsToAnalysisStep3(Input));
    return Output;
  }
}
}
