//===- LazyAndersenReversePointsToAlgorithm.cpp - reverse points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenValueInfoAlgorithmId.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ReversePointsToOutgoingRelationsAnalysisStep :
      public RelationsAnalysisStep<SOURCE> {
  public:
    explicit ReversePointsToOutgoingRelationsAnalysisStep(ValueInfo *VI)
      : RelationsAnalysisStep<SOURCE>(VI) {}

    virtual std::string getNodeLabel() const { return "ReverseOutgoingStep"; }

  protected:
    virtual AnalysisResult *analyzeRelation(Relation *R);
  };

  AnalysisResult *
  ReversePointsToOutgoingRelationsAnalysisStep::analyzeRelation(Relation *R) {
    return R->analyzeOutgoingReversePointsToSet();
  }

  class ReversePointsToIncomingRelationsAnalysisStep :
      public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ReversePointsToIncomingRelationsAnalysisStep(ValueInfo *VI)
      : RelationsAnalysisStep<DESTINATION>(VI) {}

    virtual std::string getNodeLabel() const { return "ReverseIncomingStep"; }

  protected:
    virtual AnalysisResult *analyzeRelation(Relation *R);
  };

  AnalysisResult *
  ReversePointsToIncomingRelationsAnalysisStep::analyzeRelation(Relation *R) {
    return R->analyzeIncomingReversePointsToSet();
  }
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId, REVERSE_POINTS_TO_SET>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->addValueInfo(Input);
    Output->addWork(new ReversePointsToOutgoingRelationsAnalysisStep(
        Input));
    Output->addWork(new ReversePointsToIncomingRelationsAnalysisStep(
        Input));
    return Output;
  }
}
}
