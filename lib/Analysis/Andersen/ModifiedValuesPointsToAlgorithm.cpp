//===- ModifiedValuesPointsToAlgorithm.cpp --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the modified values points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "ModifiedValuesPointsToAlgorithm.h"

namespace llvm {
namespace andersen_internal {

const LiteralAlgorithmId ModifiedValuesPointsToAlgorithm::ID(
    "modified-values");

}
}
