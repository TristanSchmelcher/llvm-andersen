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
#include "LazyAndersenRelationsAnalysisStep.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class PointsToRelationsAnalysisStep : public RelationsAnalysisStep<SOURCE> {
  public:
    explicit PointsToRelationsAnalysisStep(ValueInfo *VI)
      : RelationsAnalysisStep<SOURCE>(VI) {}

    virtual std::string getNodeLabel() const { return "PointsStep"; }

  protected:
    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzePointsToSet();
    }
  };
}

const char PointsToAlgorithm::ID[] = "points-to";

AnalysisResult *PointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new PointsToRelationsAnalysisStep(VI));
  return AR;
}
