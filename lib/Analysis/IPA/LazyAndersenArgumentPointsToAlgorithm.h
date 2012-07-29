//===- LazyAndersenArgumentPointsToAlgorithm.h - argument points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the argument value points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENARGUMENTPOINTSTOALGORITHM_H
#define LAZYANDERSENARGUMENTPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisAlgorithm.h"

namespace llvm {
namespace lazyandersen {
  class ArgumentPointsToAlgorithm :
      public AnalysisAlgorithm<AnalysisResult> {
  public:
    virtual void getLazyResult(AnalysisResult *Output,
        AnalysisResult *Input) const;
  };
}
}

#endif
