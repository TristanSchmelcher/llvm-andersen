//===- FormalReturnValueReversePointsToAlgorithm.cpp ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the formal return value reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#include "FormalReturnValueReversePointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::andersen_internal;

const LiteralAlgorithmId FormalReturnValueReversePointsToAlgorithm::ID(
    "formal return value");
