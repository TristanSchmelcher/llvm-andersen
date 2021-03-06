//===- FormalParametersReversePointsToAlgorithm.cpp -----------------------===//
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

#include "FormalParametersReversePointsToAlgorithm.h"

namespace llvm {
namespace andersen_internal {

const LiteralAlgorithmId FormalParametersReversePointsToAlgorithm::ID(
    "formal-params-reverse");

}
}
