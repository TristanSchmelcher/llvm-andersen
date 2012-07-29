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

#include "LazyAndersenReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenRelationsAnalysisStep-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ReversePointsToOutgoingRelationsAnalysisStep :
      public RelationsAnalysisStep<OUTGOING,
          ReversePointsToOutgoingRelationsAnalysisStep> {
    friend class RelationsAnalysisStep<OUTGOING,
        ReversePointsToOutgoingRelationsAnalysisStep>;

  public:
    explicit ReversePointsToOutgoingRelationsAnalysisStep(ValueInfo *VI);

  private:
    AnalysisResult *analyzeRelation(Relation *R);
  };

  inline ReversePointsToOutgoingRelationsAnalysisStep
      ::ReversePointsToOutgoingRelationsAnalysisStep(ValueInfo *VI)
    : RelationsAnalysisStep<OUTGOING,
          ReversePointsToOutgoingRelationsAnalysisStep>(VI) {}

  inline AnalysisResult *
  ReversePointsToOutgoingRelationsAnalysisStep::analyzeRelation(Relation *R) {
    // TODO
    return 0;
  }

  class ReversePointsToIncomingRelationsAnalysisStep :
      public RelationsAnalysisStep<INCOMING,
          ReversePointsToIncomingRelationsAnalysisStep> {
    friend class RelationsAnalysisStep<INCOMING,
        ReversePointsToIncomingRelationsAnalysisStep>;

  public:
    explicit ReversePointsToIncomingRelationsAnalysisStep(ValueInfo *VI);

  private:
    AnalysisResult *analyzeRelation(Relation *R);
  };

  inline ReversePointsToIncomingRelationsAnalysisStep
      ::ReversePointsToIncomingRelationsAnalysisStep(ValueInfo *VI)
    : RelationsAnalysisStep<INCOMING,
          ReversePointsToIncomingRelationsAnalysisStep>(VI) {}

  inline AnalysisResult *
  ReversePointsToIncomingRelationsAnalysisStep::analyzeRelation(Relation *R) {
    // TODO
    return 0;
  }
}

void ReversePointsToAlgorithm::getLazyResult(AnalysisResult *Output,
    ValueInfo *Input) const {
  Output->push_back(new ReversePointsToOutgoingRelationsAnalysisStep(
      Input));
  Output->push_back(new ReversePointsToIncomingRelationsAnalysisStep(
      Input));
}
