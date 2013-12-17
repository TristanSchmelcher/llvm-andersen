//===- StoredValuesPointsToAlgorithm.cpp ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the stored values points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "StoredValuesPointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::andersen_internal;

const LiteralAlgorithmId StoredValuesPointsToAlgorithm::ID("stored values");
