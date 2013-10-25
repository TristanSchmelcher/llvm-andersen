//===- LazyAndersenReturnValueReversePointsToAlgorithm.cpp ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the return value reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReturnValueReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ReturnValueReversePointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ReturnValueReversePointsToAnalysisStep2(ValueInfo *VI)
      : RelationsAnalysisStep<DESTINATION>(VI) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeReturnValueReversePointsToSet();
    }

    virtual std::string getNodeLabel() const {
      return "ReturnValueReverseStep2";
    }
  };

  struct ReturnValueReversePointsToAlgorithmStep2 {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };

  const char ReturnValueReversePointsToAlgorithmStep2::ID[] =
      "return value reverse points-to 2";

  AnalysisResult *ReturnValueReversePointsToAlgorithmStep2::run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->addWork(new ReturnValueReversePointsToAnalysisStep2(VI));
    return AR;
  }

  class ReturnValueReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ReturnValueReversePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ReturnValueReversePointsToAlgorithmStep2>();
    }

    virtual std::string getNodeLabel() const {
      return "ReturnValueReverseStep";
    }
  };
}

const char ReturnValueReversePointsToAlgorithm::ID[] =
    "return value reverse points-to";

AnalysisResult *ReturnValueReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ReturnValueReversePointsToAnalysisStep(
      VI->getAlgorithmResult<ReversePointsToAlgorithm>()));
  return AR;
}
