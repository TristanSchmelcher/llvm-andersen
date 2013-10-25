//===- LazyAndersenArgumentReversePointsToAlgorithm.h ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the argument reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENARGUMENTREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENARGUMENTREVERSEPOINTSTOALGORITHM_H

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct ArgumentReversePointsToAlgorithm {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
