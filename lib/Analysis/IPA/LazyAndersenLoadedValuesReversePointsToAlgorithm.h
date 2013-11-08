//===- LazyAndersenLoadedValuesReversePointsToAlgorithm.h -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the loaded values reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENLOADEDVALUESREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENLOADEDVALUESREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct LoadedValuesReversePointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct LoadedValuesReversePointsToAlgorithm::RelationHandler<LOADED_FROM> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<LoadedValuesReversePointsToAlgorithm,
          ReversePointsToAlgorithm>(Src);
    }
  };
}
}

#endif
