//===- LazyAndersenPointsToAlgorithm.cpp - points-to algorithm ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the main points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenPointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class PointsToRelationsAnalysisStep : public RelationsAnalysisStep<OUTGOING,
      PointsToRelationsAnalysisStep> {
    friend class RelationsAnalysisStep<OUTGOING,
        PointsToRelationsAnalysisStep>;

  public:
    explicit PointsToRelationsAnalysisStep(ValueInfo *VI);

  private:
    AnalysisResult *analyzeRelation(Relation *R);
  };

  inline PointsToRelationsAnalysisStep::PointsToRelationsAnalysisStep(
      ValueInfo *VI)
    : RelationsAnalysisStep<OUTGOING, PointsToRelationsAnalysisStep>(VI) {}

  inline AnalysisResult *PointsToRelationsAnalysisStep::analyzeRelation(
      Relation *R) {
    return R->analyzePointsToSet();
  }
}

DEFINE_ALGORITHM(ValueInfoAlgorithmId, POINTS_TO_SET, Input) {
  AnalysisResult *Output = new AnalysisResult();
  Output->push_back(new PointsToRelationsAnalysisStep(Input));
  return Output;
}
