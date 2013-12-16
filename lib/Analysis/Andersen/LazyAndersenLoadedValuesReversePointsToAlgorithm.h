//===- LazyAndersenLoadedValuesReversePointsToAlgorithm.h -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the loaded values reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENLOADEDVALUESREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENLOADEDVALUESREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenLiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {
  struct LoadedValuesReversePointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const LiteralAlgorithmId ID;
  };
}
}

#endif
