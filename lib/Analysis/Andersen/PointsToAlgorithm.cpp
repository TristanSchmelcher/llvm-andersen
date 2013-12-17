//===- PointsToAlgorithm.cpp - points-to algorithm ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the main points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "PointsToAlgorithm.h"

namespace llvm {
namespace andersen_internal {

const LiteralAlgorithmId PointsToAlgorithm::ID("self");

}
}
