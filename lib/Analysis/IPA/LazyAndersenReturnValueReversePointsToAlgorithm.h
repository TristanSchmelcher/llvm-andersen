//===- LazyAndersenReturnValueReversePointsToAlgorithm.h ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the return value reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRETURNVALUEREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENRETURNVALUEREVERSEPOINTSTOALGORITHM_H

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct ReturnValueReversePointsToAlgorithm {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
