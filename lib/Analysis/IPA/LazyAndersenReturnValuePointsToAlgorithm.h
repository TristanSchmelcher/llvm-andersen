//===- LazyAndersenReturnValuePointsToAlgorithm.h - return val points-to --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the return value points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRETURNVALUEPOINTSTOALGORITHM_H
#define LAZYANDERSENRETURNVALUEPOINTSTOALGORITHM_H

#include "LazyAndersenRelationType.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  struct ActualReturnValuePointsToAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct ActualReturnValuePointsToAlgorithm::RelationHandler<
      RETURNED_TO_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst);
  };

  // TODO: Rename to formal return value.
  struct ReturnValuePointsToAlgorithm {
    static const char ID[];
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
