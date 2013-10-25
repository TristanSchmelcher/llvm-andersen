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
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ArgumentReversePointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ArgumentReversePointsToAnalysisStep2(ValueInfo *VI)
      : RelationsAnalysisStep<DESTINATION>(VI) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeArgumentsReversePointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ArgumentReverseStep2"; }
  };

  struct ArgumentReversePointsToAlgorithmStep2 {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };

  const char ArgumentReversePointsToAlgorithmStep2::ID[] =
      "argument reverse points-to 2";

  AnalysisResult *ArgumentReversePointsToAlgorithmStep2::run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->addWork(new ArgumentReversePointsToAnalysisStep2(VI));
    return AR;
  }

  class ArgumentReversePointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ArgumentReversePointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ArgumentReversePointsToAlgorithmStep2>();
    }

    virtual std::string getNodeLabel() const { return "ArgumentReverseStep"; }
  };
}

const char ArgumentReversePointsToAlgorithm::ID[] =
    "argument reverse points-to";

AnalysisResult *ArgumentReversePointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ArgumentReversePointsToAnalysisStep(
      VI->getAlgorithmResult<PointsToAlgorithm>()));
  return AR;
}
