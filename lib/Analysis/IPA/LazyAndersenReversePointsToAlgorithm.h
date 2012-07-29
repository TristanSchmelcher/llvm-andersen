//===- LazyAndersenReversePointsToAlgorithm.h - reverse points-to ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisAlgorithm.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  class ReversePointsToAlgorithm : public AnalysisAlgorithm<ValueInfo> {
  public:
    virtual void getLazyResult(AnalysisResult *Output, ValueInfo *Input)
        const;
  };
}
}

#endif
