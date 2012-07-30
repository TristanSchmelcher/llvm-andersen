//===- LazyAndersenContentPointsToAlgorithm.h - content points-to ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the content points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENCONTENTPOINTSTOALGORITHM_H
#define LAZYANDERSENCONTENTPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisAlgorithm.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  class ContentPointsToAlgorithm :
      public AnalysisAlgorithm<AnalysisResult, AnalysisResult> {
  public:
    virtual AnalysisResult *operator()(AnalysisResult *Input) const;
  };
}
}

#endif
