//===- LazyAndersenReturnValueReversePointsToAlgorithm.cpp ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the return value reverse points-to algorithm.
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
  class ReturnValueReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ReturnValueReversePointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<RETURN_VALUE_REVERSE_POINTS_TO_SET_STEP2>();
    }

    virtual std::string getNodeLabel() const {
      return "ReturnValueReverseStep";
    }
  };

  class ReturnValueReversePointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ReturnValueReversePointsToAnalysisStep2(ValueInfo *Input)
      : RelationsAnalysisStep<DESTINATION>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeReturnValueReversePointsToSet();
    }

    virtual std::string getNodeLabel() const {
      return "ReturnValueReverseStep2";
    }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               RETURN_VALUE_REVERSE_POINTS_TO_SET>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ReturnValueReversePointsToAnalysisStep(
        Input->getAlgorithmResult<REVERSE_POINTS_TO_SET>()));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               RETURN_VALUE_REVERSE_POINTS_TO_SET_STEP2>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ReturnValueReversePointsToAnalysisStep2(Input));
    return Output;
  }
}
}
