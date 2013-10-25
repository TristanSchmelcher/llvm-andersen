//===- LazyAndersenArgumentPointsToAlgorithm.h - argument points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the argument points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENARGUMENTPOINTSTOALGORITHM_H
#define LAZYANDERSENARGUMENTPOINTSTOALGORITHM_H

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct ArgumentPointsToAlgorithm {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
