//===- LazyAndersenFormalParametersReversePointsToAlgorithm.h -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the formal parameters reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENFORMALPARAMETERSREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENFORMALPARAMETERSREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct FormalParametersReversePointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct FormalParametersReversePointsToAlgorithm::RelationHandler<
      ARGUMENT_FROM_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<FormalParametersReversePointsToAlgorithm,
          ReversePointsToAlgorithm>(Src);
    }
  };
}
}

#endif
