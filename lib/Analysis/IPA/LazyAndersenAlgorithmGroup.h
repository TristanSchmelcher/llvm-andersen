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

#include "LazyAndersenAnalysisAlgorithm.h"

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
    typedef AnalysisAlgorithm<InputTy, OutputTy> AlgorithmTy; \
\
    template<ALGORITHM_ID_TY AlgorithmId> \
    class AlgorithmImplementation; \
  }

#define DECLARE_ALGORITHM(ALGORITHM_ID_TY, ALGORITHM_ID) \
  template<> \
  class AlgorithmGroup<ALGORITHM_ID_TY>::AlgorithmImplementation<ALGORITHM_ID> \
      : public AlgorithmGroup<ALGORITHM_ID_TY>::AlgorithmTy { \
  public: \
    virtual AlgorithmGroup<ALGORITHM_ID_TY>::OutputTy *operator()( \
        AlgorithmGroup<ALGORITHM_ID_TY>::InputTy *Input) const; \
  }

#define DEFINE_ALGORITHM(ALGORITHM_ID_TY, ALGORITHM_ID, ARG_NAME) \
  AlgorithmGroup<ALGORITHM_ID_TY>::OutputTy * \
  AlgorithmGroup<ALGORITHM_ID_TY>::AlgorithmImplementation<ALGORITHM_ID> \
      ::operator()(AlgorithmGroup<ALGORITHM_ID_TY>::InputTy *ARG_NAME) const

}
}

#endif
