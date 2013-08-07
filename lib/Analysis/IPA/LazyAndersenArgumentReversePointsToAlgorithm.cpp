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

#include "LazyAndersenAnalysisResultAlgorithmId.h"

#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ArgumentReversePointsToAnalysisStep
    : public MetaAnalysisStep<ARGUMENT_REVERSE_POINTS_TO_SET> {
  public:
    explicit ArgumentReversePointsToAnalysisStep(AnalysisResult *Input)
      : MetaAnalysisStep<ARGUMENT_REVERSE_POINTS_TO_SET>(Input) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ARGUMENT_REVERSE_POINTS_TO_SET_STEP2>();
    }

    virtual const char *getWorkName() const { return "ArgumentReverseStep"; }
  };

  class ArgumentReversePointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ArgumentReversePointsToAnalysisStep2(ValueInfo *Input)
      : RelationsAnalysisStep<DESTINATION>(Input) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeArgumentsReversePointsToSet();
    }

    virtual const char *getWorkName() const { return "ArgumentReverseStep2"; }
  };
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<AnalysisResultAlgorithmId,
                               ARGUMENT_REVERSE_POINTS_TO_SET>(
      AnalysisResult *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new ArgumentReversePointsToAnalysisStep(Input));
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
