//===- LazyAndersenFormalReturnValuePointsToAlgorithm.h -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the formal return value points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENFORMALRETURNVALUEPOINTSTOALGORITHM_H
#define LAZYANDERSENFORMALRETURNVALUEPOINTSTOALGORITHM_H

#include "LazyAndersenIsNotNecessarilyEmptyIfMissingProperty.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct FormalReturnValuePointsToAlgorithm :
      public IsNotNecessarilyEmptyIfMissingProperty {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
