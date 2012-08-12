//===- LazyAndersenContentReversePointsToAlgorithm.h ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the content reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENCONTENTREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENCONTENTREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisAlgorithm.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  class ContentReversePointsToAlgorithm :
      public AnalysisAlgorithm<AnalysisResult, AnalysisResult> {
  public:
    virtual AnalysisResult *operator()(AnalysisResult *Input) const;
  };
}
}

#endif
