//===- LazyAndersenContentReversePointsToAlgorithm.h ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the content reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENCONTENTREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENCONTENTREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenAnalysisResultAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  DECLARE_ALGORITHM(AnalysisResultAlgorithmId, CONTENT_REVERSE_POINTS_TO_SET);
}
}

#endif
