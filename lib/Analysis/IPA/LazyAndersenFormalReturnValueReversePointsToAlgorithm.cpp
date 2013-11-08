//===- LazyAndersenFormalReturnValueReversePointsToAlgorithm.cpp ----------===//
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

#include "LazyAndersenFormalReturnValueReversePointsToAlgorithm.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char FormalReturnValueReversePointsToAlgorithm::ID[] =
    "formal return value reverse points-to";
