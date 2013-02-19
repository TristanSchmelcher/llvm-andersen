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
  struct AlgorithmGroup;

#define CREATE_ALGORITHM_GROUP(ALGORITHM_ID_TY, NUM_ALGORITHM_IDS, INPUT_TY, \
    OUTPUT_TY) \
  template<> \
  struct AlgorithmGroup<ALGORITHM_ID_TY> { \
    static const ALGORITHM_ID_TY NumAlgorithmIds = NUM_ALGORITHM_IDS; \
    typedef INPUT_TY InputTy; \
    typedef OUTPUT_TY OutputTy; \
    typedef OutputTy *(*AlgorithmTy)(InputTy *); \
\
    template<ALGORITHM_ID_TY AlgorithmId> \
    class AlgorithmImplementation { \
    public: \
      static OutputTy *run(InputTy *Input); \
    }; \
  }

#define DEFINE_ALGORITHM(ALGORITHM_ID_TY, ALGORITHM_ID, ARG_NAME) \
  template<> \
  AlgorithmGroup<ALGORITHM_ID_TY>::OutputTy * \
  AlgorithmGroup<ALGORITHM_ID_TY>::AlgorithmImplementation<ALGORITHM_ID> \
      ::run(AlgorithmGroup<ALGORITHM_ID_TY>::InputTy *ARG_NAME)

}
}

#endif
