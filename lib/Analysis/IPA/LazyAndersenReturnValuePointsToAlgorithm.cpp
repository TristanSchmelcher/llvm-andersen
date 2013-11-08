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

#include "LazyAndersenReturnValuePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char ActualReturnValuePointsToAlgorithm::ID[] =
    "actual return value points-to";

void ActualReturnValuePointsToAlgorithm::RelationHandler<RETURNED_TO_CALLER>
    ::onRelation(ValueInfo *Src, ValueInfo *Dst) {
  Dst->addInstructionAnalysisWork<ActualReturnValuePointsToAlgorithm,
      PointsToAlgorithm>(Src);
}

namespace {
  class ReturnValuePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ReturnValuePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<
          ActualReturnValuePointsToAlgorithm, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ReturnValueStep"; }
  };
}

const char ReturnValuePointsToAlgorithm::ID[] = "return value points-to";

AnalysisResult *ReturnValuePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ReturnValuePointsToAnalysisStep(
      VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
