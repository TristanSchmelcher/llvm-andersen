//===- LazyAndersenArgumentPointsToAlgorithm.cpp - argument points-to -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the argument points-to algorithm.
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
  class ArgumentPointsToAnalysisStep
    : public MetaAnalysisStep<ARGUMENT_POINTS_TO_SET> {
  public:
    explicit ArgumentPointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep<ARGUMENT_POINTS_TO_SET>(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ARGUMENT_POINTS_TO_SET_STEP2>();
    }

    virtual std::string getNodeLabel() const { return "ArgumentStep"; }
  };

  class ArgumentPointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ArgumentPointsToAnalysisStep2(ValueInfo *Input)
      : RelationsAnalysisStep<DESTINATION>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeArgumentsPointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ArgumentStep2"; }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               ARGUMENT_POINTS_TO_SET>(
      AnalysisResult *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ArgumentPointsToAnalysisStep(Input));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               ARGUMENT_POINTS_TO_SET_STEP2>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ArgumentPointsToAnalysisStep2(Input));
    return Output;
  }
}
}
