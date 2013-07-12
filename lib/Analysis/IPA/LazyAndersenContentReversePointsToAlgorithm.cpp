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

#include "LazyAndersenAnalysisResultAlgorithmId.h"

#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ContentReversePointsToAnalysisStep
    : public MetaAnalysisStep<CONTENT_REVERSE_POINTS_TO_SET> {
  public:
    explicit ContentReversePointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep<CONTENT_REVERSE_POINTS_TO_SET>(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<REVERSE_POINTS_TO_SET>()
          ->getAlgorithmResult<CONTENT_REVERSE_POINTS_TO_SET_STEP2>();
    }
  };

  class ContentReversePointsToAnalysisStep2
    : public MetaAnalysisStep<CONTENT_REVERSE_POINTS_TO_SET_STEP2> {
  public:
    explicit ContentReversePointsToAnalysisStep2(AnalysisResult *Input)
      : MetaAnalysisStep<CONTENT_REVERSE_POINTS_TO_SET_STEP2>(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<CONTENT_REVERSE_POINTS_TO_SET_STEP3>();
    }
  };

  class ContentReversePointsToAnalysisStep3
    : public RelationsAnalysisStep<INCOMING> {
  public:
    explicit ContentReversePointsToAnalysisStep3(ValueInfo *Input)
      : RelationsAnalysisStep<INCOMING>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeLoadedValuesReversePointsToSet();
    }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               CONTENT_REVERSE_POINTS_TO_SET>(
      AnalysisResult *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ContentReversePointsToAnalysisStep(Input));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               CONTENT_REVERSE_POINTS_TO_SET_STEP2>(
      AnalysisResult *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ContentReversePointsToAnalysisStep2(Input));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_REVERSE_POINTS_TO_SET_STEP3>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ContentReversePointsToAnalysisStep3(Input));
    return Output;
  }
}
}
