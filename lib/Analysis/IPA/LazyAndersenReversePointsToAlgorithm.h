//===- LazyAndersenReversePointsToAlgorithm.h - reverse points-to ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenValueInfoAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  DECLARE_ALGORITHM(ValueInfoAlgorithmId, REVERSE_POINTS_TO_SET);
}
}

#endif
