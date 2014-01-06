//===- StoredValuesPointsToAlgorithm.h ------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the stored values points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef STOREDVALUESPOINTSTOALGORITHM_H
#define STOREDVALUESPOINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {

struct StoredValuesPointsToAlgorithm :
    public InstructionAnalysisAlgorithm<StoredValuesPointsToAlgorithm> {
  static const LiteralAlgorithmId ID;
};

}
}

#endif
