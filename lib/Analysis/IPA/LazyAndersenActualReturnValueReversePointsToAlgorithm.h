//===- LazyAndersenActualReturnValueReversePointsToAlgorithm.h ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the actual return value reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENACTUALRETURNVALUEREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENACTUALRETURNVALUEREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenIsNotNecessarilyEmptyIfMissingProperty.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct ActualReturnValueReversePointsToAlgorithm :
      public IsNotNecessarilyEmptyIfMissingProperty {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
