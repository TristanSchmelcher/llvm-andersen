//===- LazyAndersenFormalReturnValueReversePointsToAlgorithm.h ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the formal return value reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENFORMALRETURNVALUEREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENFORMALRETURNVALUEREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct FormalReturnValueReversePointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct FormalReturnValueReversePointsToAlgorithm::RelationHandler<
      RETURNED_FROM_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<FormalReturnValueReversePointsToAlgorithm,
          ReversePointsToAlgorithm>(Src);
    }
  };
}
}

#endif
