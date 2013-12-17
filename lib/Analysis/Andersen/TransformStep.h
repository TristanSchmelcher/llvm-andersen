//===- TransformStep.h - analysis classes ---------------------------------===//
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

#ifndef TRANSFORMSTEP_H
#define TRANSFORMSTEP_H

#include "Phase.h"
#include "TransformStepBase.h"
#include "ValueInfo.h"

#include <string>

namespace llvm {
namespace andersen_internal {
  class AnalysisResult;

  template<typename AlgorithmTy>
  class TransformStep : public TransformStepBase {
  public:
    explicit TransformStep(AnalysisResult *AR) : TransformStepBase(AR) {}

    virtual std::string getNodeLabel(const Data &Data) const {
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
