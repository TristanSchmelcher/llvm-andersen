//===- LazyAndersenReturnValuePointsToAlgorithm.cpp - return val points-to ===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the return value points-to algorithm.
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
  class ReturnValuePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ReturnValuePointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<RETURN_VALUE_POINTS_TO_SET_STEP2>();
    }

    virtual std::string getNodeLabel() const { return "ReturnValueStep"; }
  };

  class ReturnValuePointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ReturnValuePointsToAnalysisStep2(ValueInfo *Input)
      : RelationsAnalysisStep<DESTINATION>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeReturnValuePointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ReturnValueStep2"; }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               RETURN_VALUE_POINTS_TO_SET>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ReturnValuePointsToAnalysisStep(
        Input->getAlgorithmResult<POINTS_TO_SET>()));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               RETURN_VALUE_POINTS_TO_SET_STEP2>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addWork(new ReturnValuePointsToAnalysisStep2(Input));
    return Output;
  }
}
}
