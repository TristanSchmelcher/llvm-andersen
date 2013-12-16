//===- LazyAndersenFormalParametersReversePointsToAlgorithm.cpp -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the formal parameters reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenFormalParametersReversePointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::andersen_internal;

const LiteralAlgorithmId FormalParametersReversePointsToAlgorithm::ID(
    "formal params reverse");
