//===- LazyAndersenAlgorithmGroup.h - algorithm classes -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares templates and macros for algorithm groups.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENALGORITHMGROUP_H
#define LAZYANDERSENALGORITHMGROUP_H

namespace llvm {
namespace lazyandersen {
  template<typename AlgorithmIdTy>
  struct AlgorithmGroupTraits;

  template<typename AlgorithmIdTy, AlgorithmIdTy AlgorithmId>
  typename AlgorithmGroupTraits<AlgorithmIdTy>::OutputTy *runAlgorithm(
      typename AlgorithmGroupTraits<AlgorithmIdTy>::InputTy *);

}
}

#endif
