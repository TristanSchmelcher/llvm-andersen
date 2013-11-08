//===- LazyAndersenPointsToAlgorithm.h - points-to algorithm --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the main points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENPOINTSTOALGORITHM_H
#define LAZYANDERSENPOINTSTOALGORITHM_H

#include "LazyAndersenContentPointsToAlgorithm.h"
#include "LazyAndersenFormalParametersPointsToAlgorithm.h"
#include "LazyAndersenFormalReturnValuePointsToAlgorithm.h"
#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct PointsToAlgorithm : public InstructionAnalysisAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct PointsToAlgorithm::RelationHandler<ARGUMENT_FROM_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<PointsToAlgorithm,
          FormalParametersPointsToAlgorithm>(Dst);
    }
  };

  template<>
  struct PointsToAlgorithm::RelationHandler<DEPENDS_ON> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<PointsToAlgorithm,
          PointsToAlgorithm>(Dst);
    }
  };

  template<>
  struct PointsToAlgorithm::RelationHandler<LOADED_FROM> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<PointsToAlgorithm,
          ContentPointsToAlgorithm>(Dst);
    }
  };

  template<>
  struct PointsToAlgorithm::RelationHandler<RETURNED_FROM_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<PointsToAlgorithm,
          FormalReturnValuePointsToAlgorithm>(Dst);
    }
  };
}
}

#endif
