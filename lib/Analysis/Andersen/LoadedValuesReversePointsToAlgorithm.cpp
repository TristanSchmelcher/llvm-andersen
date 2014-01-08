//===- LoadedValuesReversePointsToAlgorithm.cpp ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the loaded values reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LoadedValuesReversePointsToAlgorithm.h"

namespace llvm {
namespace andersen_internal {

const LiteralAlgorithmId LoadedValuesReversePointsToAlgorithm::ID(
    "loaded-values-reverse");

}
}
