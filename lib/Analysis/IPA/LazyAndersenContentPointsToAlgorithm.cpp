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
#include "LazyAndersenIsNotNecessarilyEmptyIfMissingProperty.h"
#include "LazyAndersenMetaAnalysisStep.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char StoredValuesPointsToAlgorithm::ID[] =
    "stored values points-to";

void StoredValuesPointsToAlgorithm::RelationHandler<STORED_TO>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Dst->addInstructionAnalysisWork<StoredValuesPointsToAlgorithm,
      PointsToAlgorithm>(Src);
}

namespace {
  class ContentPointsToAnalysisStep2 : public MetaAnalysisStep {
  public:
    explicit ContentPointsToAnalysisStep2(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<
          StoredValuesPointsToAlgorithm, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ContentStep2"; }
  };

  struct ContentPointsToAlgorithmStep2 :
      public IsNotNecessarilyEmptyIfMissingProperty {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };

  const char ContentPointsToAlgorithmStep2::ID[] = "content points-to 2";

  AnalysisResult *ContentPointsToAlgorithmStep2::run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->addWork(new ContentPointsToAnalysisStep2(
        VI->getAlgorithmResult<ReversePointsToAlgorithm, ENUMERATION_PHASE>()));
    return AR;
  }

  class ContentPointsToAnalysisStep : public MetaAnalysisStep {
  public:
    explicit ContentPointsToAnalysisStep(AnalysisResult *VI)
      : MetaAnalysisStep(VI) {}

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<ContentPointsToAlgorithmStep2, ENUMERATION_PHASE>();
    }

    virtual std::string getNodeLabel() const { return "ContentStep"; }
  };
}

const char ContentPointsToAlgorithm::ID[] = "content points-to";

AnalysisResult *ContentPointsToAlgorithm::run(ValueInfo *VI) {
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new ContentPointsToAnalysisStep(
      VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()));
  return AR;
}
