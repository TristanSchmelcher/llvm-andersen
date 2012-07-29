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

#ifndef LAZYANDERSENALGORITHMGROUPTRAITS_H
#define LAZYANDERSENALGORITHMGROUPTRAITS_H

namespace llvm {
namespace lazyandersen {
  template<typename AlgorithmIdTy>
  struct AlgorithmGroupTraits;

#define ALGORITHM_GROUP_TRAITS(ALGORITHM_ID_TY, NUM_ALGORITHM_IDS, INPUT_TYPE) \
  template<> \
  struct AlgorithmGroupTraits<ALGORITHM_ID_TY> { \
    static const ALGORITHM_ID_TY NumAlgorithmIds = NUM_ALGORITHM_IDS; \
    typedef INPUT_TYPE InputType; \
\
    template<ALGORITHM_ID_TY AlgorithmId> \
    struct AlgorithmTraits; \
  }

#define ALGORITHM_TRAITS(ALGORITHM_ID, ALGORITHM_TYPE) \
  template<> \
  struct AlgorithmGroupTraits<typeof ALGORITHM_ID>::AlgorithmTraits< \
      ALGORITHM_ID> { \
    typedef ALGORITHM_TYPE Type; \
  }
}
}

#endif
