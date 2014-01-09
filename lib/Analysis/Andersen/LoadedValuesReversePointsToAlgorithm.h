//===- LoadedValuesReversePointsToAlgorithm.h -----------------------------===//
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

#ifndef LOADEDVALUESREVERSEPOINTSTOALGORITHM_H
#define LOADEDVALUESREVERSEPOINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {

struct LoadedValuesReversePointsToAlgorithm :
    public InstructionAnalysisAlgorithm {
  static const LiteralAlgorithmId ID;
};

}
}

#endif
