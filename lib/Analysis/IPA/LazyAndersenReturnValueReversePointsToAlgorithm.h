//===- LazyAndersenReturnValueReversePointsToAlgorithm.h ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the return value reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRETURNVALUEREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENRETURNVALUEREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisAlgorithm.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  class ReturnValueReversePointsToAlgorithm :
      public AnalysisAlgorithm<AnalysisResult, AnalysisResult> {
  public:
    virtual AnalysisResult *operator()(AnalysisResult *Input) const;
  };
}
}

#endif
