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
#include "LazyAndersenRecursiveEnumerate.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char FormalReturnValueReversePointsToAlgorithm::ID[] =
    "formal return value reverse points-to";

void FormalReturnValueReversePointsToAlgorithm::RelationHandler<
    RETURNED_FROM_CALLEE>::onRelation(ValueInfo *Src, ValueInfo *Dst) {
  Dst->getOrCreateEagerAlgorithmResult<
      FormalReturnValueReversePointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Src->getOrCreateEagerAlgorithmResult<
                  ReversePointsToAlgorithm>()));
}

namespace {
  class ReturnValueReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ReturnValueReversePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getOrCreateEagerAlgorithmResult<
          FormalReturnValueReversePointsToAlgorithm>();
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
      VI->getOrCreateEagerAlgorithmResult<ReversePointsToAlgorithm>()));
  return AR;
}
