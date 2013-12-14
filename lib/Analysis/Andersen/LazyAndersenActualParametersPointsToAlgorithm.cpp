//===- LazyAndersenActualParametersPointsToAlgorithm.cpp ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the actual parameters points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenActualParametersPointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const LiteralAlgorithmId ActualParametersPointsToAlgorithm::ID("actual params");
