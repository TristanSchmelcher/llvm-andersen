//===- LazyAndersenArgumentReversePointsToAlgorithm.cpp -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the argument reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenArgumentReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenRecursiveEnumerate.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char FormalParametersReversePointsToAlgorithm::ID[] =
    "formal params reverse points-to";

void FormalParametersReversePointsToAlgorithm::RelationHandler<
    ARGUMENT_FROM_CALLER>::onRelation(ValueInfo *Src, ValueInfo *Dst) {
  Dst->getOrCreateEagerAlgorithmResult<
      FormalParametersReversePointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Src->getOrCreateEagerAlgorithmResult<
                  ReversePointsToAlgorithm>()));
}

namespace {
  class ArgumentReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ArgumentReversePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getOrCreateEagerAlgorithmResult<
          FormalParametersReversePointsToAlgorithm>();
    }

    virtual std::string getNodeLabel() const { return "ArgumentReverseStep"; }
  };
}

const char ArgumentReversePointsToAlgorithm::ID[] =
    "argument reverse points-to";

AnalysisResult *ArgumentReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ArgumentReversePointsToAnalysisStep(
      VI->getOrCreateEagerAlgorithmResult<PointsToAlgorithm>()));
  return AR;
}
