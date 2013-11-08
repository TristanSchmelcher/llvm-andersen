//===- LazyAndersenFormalParametersPointsToAlgorithm.h --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the formal parameters points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENFORMALPARAMETERSPOINTSTOALGORITHM_H
#define LAZYANDERSENFORMALPARAMETERSPOINTSTOALGORITHM_H

#include "LazyAndersenIsNotNecessarilyEmptyIfMissingProperty.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct FormalParametersPointsToAlgorithm :
      public IsNotNecessarilyEmptyIfMissingProperty {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
