//===- LazyAndersenArgumentReversePointsToAlgorithm.cpp -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the argument reverse points-to algorithm.
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
  class ArgumentReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ArgumentReversePointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ARGUMENT_REVERSE_POINTS_TO_SET_STEP2>();
    }

    virtual std::string getNodeLabel() const { return "ArgumentReverseStep"; }
  };

  class ArgumentReversePointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ArgumentReversePointsToAnalysisStep2(ValueInfo *Input)
      : RelationsAnalysisStep<DESTINATION>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeArgumentsReversePointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ArgumentReverseStep2"; }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               ARGUMENT_REVERSE_POINTS_TO_SET>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ArgumentReversePointsToAnalysisStep(
        Input->getAlgorithmResult<POINTS_TO_SET>()));
    return Output;
  }

  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId,
                               ARGUMENT_REVERSE_POINTS_TO_SET_STEP2>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ArgumentReversePointsToAnalysisStep2(Input));
    return Output;
  }
}
}
