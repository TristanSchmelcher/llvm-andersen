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

#include "LazyAndersenValueInfoAlgorithmId.h"

#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ContentReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ContentReversePointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<CONTENT_REVERSE_POINTS_TO_SET_STEP2>();
    }

    virtual std::string getNodeLabel() const { return "ContentReverseStep"; }
  };

  class ContentReversePointsToAnalysisStep2 : public MetaAnalysisStep {
  public:
    explicit ContentReversePointsToAnalysisStep2(AnalysisResult *Input)
      : MetaAnalysisStep(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<CONTENT_REVERSE_POINTS_TO_SET_STEP3>();
    }

    virtual std::string getNodeLabel() const { return "ContentReverseStep2"; }
  };

  class ContentReversePointsToAnalysisStep3
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ContentReversePointsToAnalysisStep3(ValueInfo *Input)
      : RelationsAnalysisStep<DESTINATION>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeLoadedValuesReversePointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ContentReverseStep3"; }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_REVERSE_POINTS_TO_SET>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ContentReversePointsToAnalysisStep(
        Input->getAlgorithmResult<POINTS_TO_SET>()));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_REVERSE_POINTS_TO_SET_STEP2>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ContentReversePointsToAnalysisStep2(
        Input->getAlgorithmResult<REVERSE_POINTS_TO_SET>()));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               CONTENT_REVERSE_POINTS_TO_SET_STEP3>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ContentReversePointsToAnalysisStep3(Input));
    return Output;
  }
}
}
