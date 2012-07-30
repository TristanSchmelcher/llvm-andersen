//===- LazyAndersenPointsToAlgorithm.h - points-to algorithm --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the main points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENPOINTSTOALGORITHM_H
#define LAZYANDERSENPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisAlgorithm.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  class PointsToAlgorithm :
      public AnalysisAlgorithm<ValueInfo, AnalysisResult> {
  public:
    virtual AnalysisResult *operator()(ValueInfo *Input) const;
  };
}
}

#endif
