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
  template<typename InputTy, typename OutputTy>
  class AnalysisAlgorithm {
  public:
    virtual OutputTy *operator()(InputTy *Input) const = 0;
  };
}
}

#endif
