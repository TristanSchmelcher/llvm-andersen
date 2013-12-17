//===- ActualParametersPointsToAlgorithm.h --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the actual parameters points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef ACTUALPARAMETERSPOINTSTOALGORITHM_H
#define ACTUALPARAMETERSPOINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {
  struct ActualParametersPointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const LiteralAlgorithmId ID;
  };
}
}

#endif
