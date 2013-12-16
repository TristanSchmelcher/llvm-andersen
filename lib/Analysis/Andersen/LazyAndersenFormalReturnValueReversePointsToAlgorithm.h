//===- LazyAndersenFormalReturnValueReversePointsToAlgorithm.h ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the formal return value reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENFORMALRETURNVALUEREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENFORMALRETURNVALUEREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenLiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {
  struct FormalReturnValueReversePointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const LiteralAlgorithmId ID;
  };
}
}

#endif
