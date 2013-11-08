//===- LazyAndersenStoredValuesPointsToAlgorithm.cpp ----------------------===//
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

#include "LazyAndersenStoredValuesPointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char StoredValuesPointsToAlgorithm::ID[] =
    "stored values points-to";
