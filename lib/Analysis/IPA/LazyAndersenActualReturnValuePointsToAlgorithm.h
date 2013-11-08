//===- LazyAndersenActualReturnValuePointsToAlgorithm.h -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the actual return value points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENACTUALRETURNVALUEPOINTSTOALGORITHM_H
#define LAZYANDERSENACTUALRETURNVALUEPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct ActualReturnValuePointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct ActualReturnValuePointsToAlgorithm::RelationHandler<
      RETURNED_TO_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<ActualReturnValuePointsToAlgorithm,
          PointsToAlgorithm>(Src);
    }
  };
}
}

#endif
