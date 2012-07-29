//===- LazyAndersenValueInfoAlgorithms.h - algorithm classes --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the group of algorithms that operate over ValueInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENVALUEINFOALGORITHMS_H
#define LAZYANDERSENVALUEINFOALGORITHMS_H

#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfoAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  ALGORITHM_TRAITS(POINTS_TO_SET, PointsToAlgorithm);
  ALGORITHM_TRAITS(REVERSE_POINTS_TO_SET, ReversePointsToAlgorithm);
}
}

#endif
