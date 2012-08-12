//===- LazyAndersenContentPointsToAlgorithm.h - content points-to ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the content points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENCONTENTPOINTSTOALGORITHM_H
#define LAZYANDERSENCONTENTPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisResultAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  DECLARE_ALGORITHM(AnalysisResultAlgorithmId, CONTENT_POINTS_TO_SET);
}
}

#endif
