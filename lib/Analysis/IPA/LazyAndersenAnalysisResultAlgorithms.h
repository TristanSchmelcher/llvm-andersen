//===- LazyAndersenAnalysisResultAlgorithms.h - algorithm classes ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the group of algorithms that operate over AnalysisResult.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTALGORITHMS_H
#define LAZYANDERSENANALYSISRESULTALGORITHMS_H

#include "LazyAndersenAnalysisResultAlgorithmId.h"
#include "LazyAndersenArgumentPointsToAlgorithm.h"
#include "LazyAndersenContentPointsToAlgorithm.h"
#include "LazyAndersenReturnValuePointsToAlgorithm.h"

namespace llvm {
namespace lazyandersen {
  ALGORITHM_TRAITS(CONTENT_POINTS_TO_SET, ContentPointsToAlgorithm);
  ALGORITHM_TRAITS(RETURN_VALUE_POINTS_TO_SET, ReturnValuePointsToAlgorithm);
  ALGORITHM_TRAITS(ARGUMENT_POINTS_TO_SET, ArgumentPointsToAlgorithm);
}
}

#endif
