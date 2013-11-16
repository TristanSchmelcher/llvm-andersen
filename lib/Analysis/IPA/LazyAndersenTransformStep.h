//===- LazyAndersenTransformStep.h - analysis classes ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a template type for transforms that compute the list
// comprehension of a ValueInfo-to-AnalysisResult function run on the elements
// of an input AnalysisResult.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENTRANSFORMSTEP_H
#define LAZYANDERSENTRANSFORMSTEP_H

#include "LazyAndersenPhase.h"
#include "LazyAndersenTransformStepBase.h"
#include "LazyAndersenValueInfo.h"

#include <string>

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  template<typename AlgorithmTy>
  class TransformStep : public TransformStepBase {
  public:
    explicit TransformStep(AnalysisResult *AR) : TransformStepBase(AR) {}

    virtual std::string getNodeLabel() const {
      return buildTransformStepName(AlgorithmTy::ID);
    }

  protected:
    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
      return VI->getAlgorithmResult<AlgorithmTy, ENUMERATION_PHASE>();
    }
  };
}
}

#endif
