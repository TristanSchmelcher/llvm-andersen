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
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ArgumentPointsToAnalysisStep2
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ArgumentPointsToAnalysisStep2(ValueInfo *VI)
      : RelationsAnalysisStep<DESTINATION>(VI) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeArgumentsPointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ArgumentStep2"; }
  };

  struct ArgumentPointsToAlgorithmStep2 {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };

  const char ArgumentPointsToAlgorithmStep2::ID[] = "argument points-to 2";

  AnalysisResult *ArgumentPointsToAlgorithmStep2::run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->addWork(new ArgumentPointsToAnalysisStep2(VI));
    return AR;
  }

  class ArgumentPointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ArgumentPointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ArgumentPointsToAlgorithmStep2>();
    }

    virtual std::string getNodeLabel() const { return "ArgumentStep"; }
  };
}

const char ArgumentPointsToAlgorithm::ID[] = "argument points-to";

AnalysisResult *ArgumentPointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ArgumentPointsToAnalysisStep(
      VI->getAlgorithmResult<ReversePointsToAlgorithm>()));
  return AR;
}
