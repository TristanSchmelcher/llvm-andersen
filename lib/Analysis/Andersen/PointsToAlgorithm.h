//===- PointsToAlgorithm.h - points-to algorithm --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the main points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef POINTSTOALGORITHM_H
#define POINTSTOALGORITHM_H

#include "InstructionAnalysisAlgorithm.h"
#include "LiteralAlgorithmId.h"

namespace llvm {
namespace andersen_internal {

struct PointsToAlgorithm :
    public InstructionAnalysisAlgorithm<PointsToAlgorithm> {
  static const LiteralAlgorithmId ID;
};

}
}

#endif
