//===- ActualReturnValuePointsToAlgorithm.h -------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the actual return value points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef ACTUALRETURNVALUEPOINTSTOALGORITHM_H
#define ACTUALRETURNVALUEPOINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {

struct ActualReturnValuePointsToAlgorithm :
    public InstructionAnalysisAlgorithm<ActualReturnValuePointsToAlgorithm> {
  static const LiteralAlgorithmId ID;
};

}
}

#endif
