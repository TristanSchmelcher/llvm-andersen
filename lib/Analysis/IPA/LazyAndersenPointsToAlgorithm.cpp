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

#include "LazyAndersenValueInfoAlgorithmId.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class PointsToRelationsAnalysisStep : public RelationsAnalysisStep<SOURCE> {
  public:
    explicit PointsToRelationsAnalysisStep(ValueInfo *VI)
      : RelationsAnalysisStep<SOURCE>(VI) {}

  protected:
    virtual AnalysisResult *analyzeRelation(Relation *R);
  };

  AnalysisResult *PointsToRelationsAnalysisStep::analyzeRelation(Relation *R) {
    return R->analyzePointsToSet();
  }
}

namespace llvm {
namespace lazyandersen {
  template<>
  AnalysisResult *runAlgorithm<ValueInfoAlgorithmId, POINTS_TO_SET>(
      ValueInfo *Input) {
    AnalysisResult *Output = new AnalysisResult();
    Output->push_back(new PointsToRelationsAnalysisStep(Input));
    return Output;
  }
}
}
