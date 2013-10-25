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
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ReturnValuePointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ReturnValuePointsToAnalysisStep2(ValueInfo *VI)
      : RelationsAnalysisStep<DESTINATION>(VI) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeReturnValuePointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ReturnValueStep2"; }
  };

  struct ReturnValuePointsToAlgorithmStep2 {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };

  const char ReturnValuePointsToAlgorithmStep2::ID[] =
      "return value points-to 2";

  AnalysisResult *ReturnValuePointsToAlgorithmStep2::run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->addWork(new ReturnValuePointsToAnalysisStep2(VI));
    return AR;
  }

  class ReturnValuePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ReturnValuePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ReturnValuePointsToAlgorithmStep2>();
    }

    virtual std::string getNodeLabel() const { return "ReturnValueStep"; }
  };
}

const char ReturnValuePointsToAlgorithm::ID[] = "return value points-to";

AnalysisResult *ReturnValuePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ReturnValuePointsToAnalysisStep(
      VI->getAlgorithmResult<PointsToAlgorithm>()));
  return AR;
}
