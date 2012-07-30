//===- LazyAndersenAlgorithmTraits.h - algorithm classes ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares templates and macros for algorithm traits specialization.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENALGORITHMTRAITS_H
#define LAZYANDERSENALGORITHMTRAITS_H

namespace llvm {
namespace lazyandersen {
  template<typename AlgorithmIdTy>
  struct AlgorithmGroupTraits;

#define DECLARE_ALGORITHM_GROUP(ALGORITHM_ID_TY, NUM_ALGORITHM_IDS, INPUT_TY, \
    OUTPUT_TY) \
  template<> \
  struct AlgorithmGroupTraits<ALGORITHM_ID_TY> { \
    static const ALGORITHM_ID_TY NumAlgorithmIds = NUM_ALGORITHM_IDS; \
    typedef INPUT_TY InputTy; \
    typedef OUTPUT_TY OutputTy; \
\
    template<ALGORITHM_ID_TY AlgorithmId> \
    struct AlgorithmTraits; \
  }

#define DECLARE_ALGORITHM(ALGORITHM_ID, ALGORITHM_TY) \
  template<> \
  struct AlgorithmGroupTraits<typeof ALGORITHM_ID>::AlgorithmTraits< \
      ALGORITHM_ID> { \
    typedef ALGORITHM_TY AlgorithmTy; \
  }
}
}

#endif
