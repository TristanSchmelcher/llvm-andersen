//===- TransformWork.h - analysis classes ---------------------------------===//
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

#ifndef TRANSFORMWORK_H
#define TRANSFORMWORK_H

#include "Phase.h"
#include "TransformWorkBase.h"
#include "ValueInfo.h"

#include <string>

namespace llvm {
namespace andersen_internal {

class AnalysisResult;

template<typename AlgorithmTy>
class TransformWork : public TransformWorkBase {
public:
  explicit TransformWork(AnalysisResult *AR) : TransformWorkBase(AR) {}

  virtual const AlgorithmId *getAlgorithmId() const {
    return &AlgorithmTy::ID;
  }

protected:
  virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) {
    return VI->getAlgorithmResult<AlgorithmTy, ENUMERATION_PHASE>();
  }
};

}
}

#endif
