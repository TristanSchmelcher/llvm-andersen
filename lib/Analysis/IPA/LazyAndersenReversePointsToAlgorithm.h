//===- LazyAndersenReversePointsToAlgorithm.h ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenRelationType.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  struct ReversePointsToAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct ReversePointsToAlgorithm::RelationHandler<ARGUMENT_TO_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst);
  };

  template<>
  struct ReversePointsToAlgorithm::RelationHandler<DEPENDS_ON> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst);
  };

  template<>
  struct ReversePointsToAlgorithm::RelationHandler<RETURNED_TO_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst);
  };

  template<>
  struct ReversePointsToAlgorithm::RelationHandler<STORED_TO> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst);
  };
}
}

#endif
