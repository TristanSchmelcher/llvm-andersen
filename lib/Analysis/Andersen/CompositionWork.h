//===- CompositionWork.h - analysis classes ---------------------------------===//
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

#ifndef COMPOSITIONWORK_H
#define COMPOSITIONWORK_H

#include "Phase.h"
#include "CompositionWorkBase.h"
#include "ValueInfo.h"

#include <string>

namespace llvm {
namespace andersen_internal {

class AnalysisResult;

template<typename AlgorithmTy>
class CompositionWork : public CompositionWorkBase {
public:
  explicit CompositionWork(AnalysisResult *AR) : CompositionWorkBase(AR) {}

  virtual const AlgorithmId *getAlgorithmId() const {
    return &AlgorithmTy::ID;
  }

protected:
  virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) const {
    return VI->getAlgorithmResult<ENUMERATION_PHASE>(AlgorithmTy());
  }
};

}
}

#endif
