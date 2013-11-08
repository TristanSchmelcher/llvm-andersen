//===- LazyAndersenActualParametersPointsToAlgorithm.h --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the actual parameters points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENACTUALPARAMETERSPOINTSTOALGORITHM_H
#define LAZYANDERSENACTUALPARAMETERSPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct ActualParametersPointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct ActualParametersPointsToAlgorithm::RelationHandler<
      ARGUMENT_TO_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<ActualParametersPointsToAlgorithm,
          PointsToAlgorithm>(Src);
    }
  };
}
}

#endif
