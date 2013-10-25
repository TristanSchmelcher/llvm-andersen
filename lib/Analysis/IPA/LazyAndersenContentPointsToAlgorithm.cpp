//===- LazyAndersenContentPointsToAlgorithm.cpp - content points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the content points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenContentPointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenRelationsAnalysisStep.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class ContentPointsToAnalysisStep3
    : public RelationsAnalysisStep<DESTINATION> {
  public:
    explicit ContentPointsToAnalysisStep3(ValueInfo *VI)
      : RelationsAnalysisStep<DESTINATION>(VI) {}

    virtual AnalysisResult *analyzeRelation(Relation *R) {
      return R->analyzeStoredValuesPointsToSet();
    }

    virtual std::string getNodeLabel() const { return "ContentStep3"; }
  };

  struct ContentPointsToAlgorithmStep3 {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };

  const char ContentPointsToAlgorithmStep3::ID[] = "content points-to 3";

  AnalysisResult *ContentPointsToAlgorithmStep3::run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->addWork(new ContentPointsToAnalysisStep3(VI));
    return AR;
  }

  class ContentPointsToAnalysisStep2 : public MetaAnalysisStep {
  public:
    explicit ContentPointsToAnalysisStep2(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ContentPointsToAlgorithmStep3>();
    }

    virtual std::string getNodeLabel() const { return "ContentStep2"; }
  };

  struct ContentPointsToAlgorithmStep2 {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };

  const char ContentPointsToAlgorithmStep2::ID[] = "content points-to 2";

  AnalysisResult *ContentPointsToAlgorithmStep2::run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->addWork(new ContentPointsToAnalysisStep2(
        VI->getAlgorithmResult<ReversePointsToAlgorithm>()));
    return AR;
  }

  class ContentPointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ContentPointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ContentPointsToAlgorithmStep2>();
    }

    virtual std::string getNodeLabel() const { return "ContentStep"; }
  };
}

const char ContentPointsToAlgorithm::ID[] = "content points-to";

AnalysisResult *ContentPointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ContentPointsToAnalysisStep(
      VI->getAlgorithmResult<PointsToAlgorithm>()));
  return AR;
}
