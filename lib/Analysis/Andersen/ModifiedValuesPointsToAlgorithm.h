//===- ModifiedValuesPointsToAlgorithm.h ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the modified values points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef MODIFIEDVALUESPOINTSTOALGORITHM_H
#define MODIFIEDVALUESPOINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {

struct ModifiedValuesPointsToAlgorithm : public InstructionAnalysisAlgorithm {
  static const LiteralAlgorithmId ID;
};

}
}

#endif
