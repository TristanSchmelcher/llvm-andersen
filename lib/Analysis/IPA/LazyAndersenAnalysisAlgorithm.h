//===- LazyAndersenAnalysisAlgorithm.h - algorithm classes ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares an object-oriented abstraction for algorithms.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISALGORITHM_H
#define LAZYANDERSENANALYSISALGORITHM_H

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  template<typename InputTy>
  class AnalysisAlgorithm {
  public:
    virtual void getLazyResult(AnalysisResult *Output, InputTy *Input)
        const = 0;
  };
}
}

#endif
