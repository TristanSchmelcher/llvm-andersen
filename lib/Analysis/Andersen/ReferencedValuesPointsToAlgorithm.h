//===- ReferencedValuesPointsToAlgorithm.h --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the referenced values points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef REFERENCEDVALUESPOINTSTOALGORITHM_H
#define REFERENCEDVALUESPOINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {

struct ReferencedValuesPointsToAlgorithm : public InstructionAnalysisAlgorithm {
  static const LiteralAlgorithmId ID;
};

}
}

#endif
