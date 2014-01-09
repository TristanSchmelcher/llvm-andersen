//===- FormalReturnValueReversePointsToAlgorithm.h ------------------------===//
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

#ifndef FORMALRETURNVALUEREVERSEPOINTSTOALGORITHM_H
#define FORMALRETURNVALUEREVERSEPOINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {

struct FormalReturnValueReversePointsToAlgorithm :
    public InstructionAnalysisAlgorithm {
  static const LiteralAlgorithmId ID;
};

}
}

#endif
