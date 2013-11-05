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

#include "LazyAndersenRelationType.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct FormalReturnValueReversePointsToAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct FormalReturnValueReversePointsToAlgorithm::RelationHandler<
      RETURNED_FROM_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst);
  };

  // TODO: Rename to actual return value.
  struct ReturnValueReversePointsToAlgorithm {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
