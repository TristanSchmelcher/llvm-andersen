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

#include "LazyAndersenArgumentPointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char ActualParametersPointsToAlgorithm::ID[] = "actual params points-to";

void ActualParametersPointsToAlgorithm::RelationHandler<ARGUMENT_TO_CALLEE>
    ::onRelation(ValueInfo *Src, ValueInfo *Dst) {
  Dst->addInstructionAnalysisWork<ActualParametersPointsToAlgorithm,
      PointsToAlgorithm>(Src);
}

namespace {
  class ArgumentPointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ArgumentPointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<
          ActualParametersPointsToAlgorithm, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ArgumentStep"; }
  };
}

const char ArgumentPointsToAlgorithm::ID[] = "argument points-to";

AnalysisResult *ArgumentPointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ArgumentPointsToAnalysisStep(
      VI->getAlgorithmResult<ReversePointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
