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

using namespace llvm;
using namespace llvm::andersen_internal;

const LiteralAlgorithmId LoadedValuesReversePointsToAlgorithm::ID(
    "loaded values reverse");
