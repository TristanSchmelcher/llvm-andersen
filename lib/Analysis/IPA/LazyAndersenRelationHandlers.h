//===- LazyAndersenRelationHandlers.h -------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the nested RelationHandler types of each instruction
// analysis algorithm. (They cannot be defined together with the algorithm
// types due to cyclic dependencies.)
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONHANDLERS_H
#define LAZYANDERSENRELATIONHANDLERS_H

#include "LazyAndersenActualParametersPointsToAlgorithm.h"
#include "LazyAndersenActualReturnValuePointsToAlgorithm.h"
#include "LazyAndersenFormalParametersReversePointsToAlgorithm.h"
#include "LazyAndersenFormalReturnValueReversePointsToAlgorithm.h"
#include "LazyAndersenLoadedValuesReversePointsToAlgorithm.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenStoredValuesPointsToAlgorithm.h"
#include "LazyAndersenTraversalAlgorithm.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  template<RelationType RT>
  struct ActualParametersPointsToAlgorithm::RelationHandler {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  struct ActualParametersPointsToAlgorithm::RelationHandler<
      ARGUMENT_TO_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<ActualParametersPointsToAlgorithm,
          PointsToAlgorithm>(Src);
    }
  };


  template<RelationType RT>
  struct ActualReturnValuePointsToAlgorithm::RelationHandler {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  struct ActualReturnValuePointsToAlgorithm::RelationHandler<
      RETURNED_TO_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<ActualReturnValuePointsToAlgorithm,
          PointsToAlgorithm>(Src);
    }
  };


  template<RelationType RT>
  struct FormalParametersReversePointsToAlgorithm::RelationHandler {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  struct FormalParametersReversePointsToAlgorithm::RelationHandler<
      ARGUMENT_FROM_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<FormalParametersReversePointsToAlgorithm,
          ReversePointsToAlgorithm>(Src);
    }
  };


  template<RelationType RT>
  struct FormalReturnValueReversePointsToAlgorithm::RelationHandler {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  struct FormalReturnValueReversePointsToAlgorithm::RelationHandler<
      RETURNED_FROM_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<FormalReturnValueReversePointsToAlgorithm,
          ReversePointsToAlgorithm>(Src);
    }
  };


  template<RelationType RT>
  struct LoadedValuesReversePointsToAlgorithm::RelationHandler {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  struct LoadedValuesReversePointsToAlgorithm::RelationHandler<LOADED_FROM> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<LoadedValuesReversePointsToAlgorithm,
          ReversePointsToAlgorithm>(Src);
    }
  };


  template<RelationType RT>
  struct PointsToAlgorithm::RelationHandler {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  struct PointsToAlgorithm::RelationHandler<ARGUMENT_FROM_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<
          PointsToAlgorithm,
          TwoHopTraversal<ReversePointsToAlgorithm,
                          ActualParametersPointsToAlgorithm>::Algorithm>(Dst);
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
      Src->addInstructionAnalysisWork<
          PointsToAlgorithm,
          ThreeHopTraversal<PointsToAlgorithm,
                            ReversePointsToAlgorithm,
                            StoredValuesPointsToAlgorithm>::Algorithm>(Dst);
    }
  };

  template<>
  struct PointsToAlgorithm::RelationHandler<RETURNED_FROM_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<
          PointsToAlgorithm,
          TwoHopTraversal<PointsToAlgorithm,
                          ActualReturnValuePointsToAlgorithm>::Algorithm>(Dst);
    }
  };


  template<RelationType RT>
  struct ReversePointsToAlgorithm::RelationHandler {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  struct ReversePointsToAlgorithm::RelationHandler<ARGUMENT_TO_CALLEE> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<
          ReversePointsToAlgorithm,
          TwoHopTraversal<PointsToAlgorithm,
                          FormalParametersReversePointsToAlgorithm>::Algorithm>(
              Dst);
    }
  };

  template<>
  struct ReversePointsToAlgorithm::RelationHandler<DEPENDS_ON> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<ReversePointsToAlgorithm,
          ReversePointsToAlgorithm>(Src);
    }
  };

  template<>
  struct ReversePointsToAlgorithm::RelationHandler<RETURNED_TO_CALLER> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<
          ReversePointsToAlgorithm,
          TwoHopTraversal<
              ReversePointsToAlgorithm,
              FormalReturnValueReversePointsToAlgorithm>::Algorithm>(Dst);
    }
  };

  template<>
  struct ReversePointsToAlgorithm::RelationHandler<STORED_TO> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Src->addInstructionAnalysisWork<
          ReversePointsToAlgorithm,
          ThreeHopTraversal<
              PointsToAlgorithm,
              ReversePointsToAlgorithm,
              LoadedValuesReversePointsToAlgorithm>::Algorithm>(Dst);
    }
  };


  template<RelationType RT>
  struct StoredValuesPointsToAlgorithm::RelationHandler {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  struct StoredValuesPointsToAlgorithm::RelationHandler<STORED_TO> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<StoredValuesPointsToAlgorithm,
          PointsToAlgorithm>(Src);
    }
  };
}
}

#endif
