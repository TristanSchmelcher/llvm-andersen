//===- LazyAndersenReversePointsToAlgorithm.h ------------------===//
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

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct ReversePointsToAlgorithm {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
