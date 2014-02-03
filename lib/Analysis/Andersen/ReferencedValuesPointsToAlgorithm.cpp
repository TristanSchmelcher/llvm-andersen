//===- ReferencedValuesPointsToAlgorithm.cpp ------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the referenced values points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "ReferencedValuesPointsToAlgorithm.h"

namespace llvm {
namespace andersen_internal {

const LiteralAlgorithmId ReferencedValuesPointsToAlgorithm::ID(
    "referenced-values");

}
}
