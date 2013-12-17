//===- FormalParametersReversePointsToAlgorithm.h -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the formal parameters reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef FORMALPARAMETERSREVERSEPOINTSTOALGORITHM_H
#define FORMALPARAMETERSREVERSEPOINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {

struct FormalParametersReversePointsToAlgorithm :
    public InstructionAnalysisAlgorithm {
  static const LiteralAlgorithmId ID;
};

}
}

#endif
