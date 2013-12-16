//===- LazyAndersenActualReturnValuePointsToAlgorithm.cpp -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the actual return value points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenActualReturnValuePointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::andersen_internal;

const LiteralAlgorithmId ActualReturnValuePointsToAlgorithm::ID(
    "actual return value");