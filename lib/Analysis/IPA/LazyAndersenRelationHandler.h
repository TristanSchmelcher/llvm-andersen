//===- LazyAndersenRelationHandler.h --------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the logic for translating the relations produced by
// instruction analysis into graph traversal steps.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONHANDLER_H
#define LAZYANDERSENRELATIONHANDLER_H

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
  class RelationHandler {
  public:
    // Creates graph traversal steps to handle the relation RT between Src and
    // Dst.
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst);

  private:
    template<typename AlgorithmTy>
    struct ForAlgorithm;
  };

  // ActualParametersPointsToAlgorithm
  template<>
  struct RelationHandler::ForAlgorithm<ActualParametersPointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void RelationHandler::ForAlgorithm<ActualParametersPointsToAlgorithm>
      ::handleRelation<ARGUMENT_TO_CALLEE>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<ActualParametersPointsToAlgorithm,
        PointsToAlgorithm>(Src);
  }

  // ActualReturnValuePointsToAlgorithm
  template<>
  struct RelationHandler::ForAlgorithm<ActualReturnValuePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void RelationHandler::ForAlgorithm<ActualReturnValuePointsToAlgorithm>
      ::handleRelation<RETURNED_TO_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<ActualReturnValuePointsToAlgorithm,
        PointsToAlgorithm>(Src);
  }

  // FormalParametersReversePointsToAlgorithm
  template<>
  struct RelationHandler
      ::ForAlgorithm<FormalParametersReversePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void RelationHandler
      ::ForAlgorithm<FormalParametersReversePointsToAlgorithm>
      ::handleRelation<ARGUMENT_FROM_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<FormalParametersReversePointsToAlgorithm,
        ReversePointsToAlgorithm>(Src);
  }

  // FormalReturnValueReversePointsToAlgorithm
  template<>
  struct RelationHandler
      ::ForAlgorithm<FormalReturnValueReversePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void RelationHandler
      ::ForAlgorithm<FormalReturnValueReversePointsToAlgorithm>
      ::handleRelation<RETURNED_FROM_CALLEE>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<FormalReturnValueReversePointsToAlgorithm,
        ReversePointsToAlgorithm>(Src);
  }

  // LoadedValuesReversePointsToAlgorithm
  template<>
  struct RelationHandler::ForAlgorithm<LoadedValuesReversePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void RelationHandler
      ::ForAlgorithm<LoadedValuesReversePointsToAlgorithm>
      ::handleRelation<LOADED_FROM>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<LoadedValuesReversePointsToAlgorithm,
        ReversePointsToAlgorithm>(Src);
  }

  // PointsToAlgorithm
  template<>
  struct RelationHandler::ForAlgorithm<PointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void RelationHandler::ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<ARGUMENT_FROM_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        PointsToAlgorithm,
        TwoHopTraversal<ReversePointsToAlgorithm,
                        ActualParametersPointsToAlgorithm>::Algorithm>(Dst);
  }

  template<>
  inline void RelationHandler::ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<DEPENDS_ON>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<PointsToAlgorithm,
        PointsToAlgorithm>(Dst);
  }

  template<>
  inline void RelationHandler::ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<LOADED_FROM>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        PointsToAlgorithm,
        ThreeHopTraversal<PointsToAlgorithm,
                          ReversePointsToAlgorithm,
                          StoredValuesPointsToAlgorithm>::Algorithm>(Dst);
  }

  template<>
  inline void RelationHandler::ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<RETURNED_FROM_CALLEE>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        PointsToAlgorithm,
        TwoHopTraversal<PointsToAlgorithm,
                        ActualReturnValuePointsToAlgorithm>::Algorithm>(Dst);
  }

  // ReversePointsToAlgorithm
  template<>
  struct RelationHandler::ForAlgorithm<ReversePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void RelationHandler::ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<ARGUMENT_TO_CALLEE>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        ReversePointsToAlgorithm,
        TwoHopTraversal<PointsToAlgorithm,
                        FormalParametersReversePointsToAlgorithm>::Algorithm>(
            Dst);
  }

  template<>
  inline void RelationHandler::ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<DEPENDS_ON>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<ReversePointsToAlgorithm,
        ReversePointsToAlgorithm>(Src);
  }

  template<>
  inline void RelationHandler::ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<RETURNED_TO_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        ReversePointsToAlgorithm,
        TwoHopTraversal<
            ReversePointsToAlgorithm,
            FormalReturnValueReversePointsToAlgorithm>::Algorithm>(Dst);
  }

  template<>
  inline void RelationHandler::ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<STORED_TO>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        ReversePointsToAlgorithm,
        ThreeHopTraversal<
            PointsToAlgorithm,
            ReversePointsToAlgorithm,
            LoadedValuesReversePointsToAlgorithm>::Algorithm>(Dst);
  }

  // StoredValuesPointsToAlgorithm
  template<>
  struct RelationHandler::ForAlgorithm<StoredValuesPointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void RelationHandler::ForAlgorithm<StoredValuesPointsToAlgorithm>
      ::handleRelation<STORED_TO>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<StoredValuesPointsToAlgorithm,
        PointsToAlgorithm>(Src);
  }

  // Now put it all together.
  template<RelationType RT>
  void RelationHandler::handleRelation(ValueInfo *Src, ValueInfo *Dst) {
    assert(Src);
    assert(Dst);
    ForAlgorithm<ActualParametersPointsToAlgorithm>
        ::handleRelation<RT>(Src, Dst);
    ForAlgorithm<ActualReturnValuePointsToAlgorithm>
        ::handleRelation<RT>(Src, Dst);
    ForAlgorithm<FormalParametersReversePointsToAlgorithm>
        ::handleRelation<RT>(Src, Dst);
    ForAlgorithm<FormalReturnValueReversePointsToAlgorithm>
        ::handleRelation<RT>(Src, Dst);
    ForAlgorithm<LoadedValuesReversePointsToAlgorithm>
        ::handleRelation<RT>(Src, Dst);
    ForAlgorithm<PointsToAlgorithm>
        ::handleRelation<RT>(Src, Dst);
    ForAlgorithm<ReversePointsToAlgorithm>
        ::handleRelation<RT>(Src, Dst);
    ForAlgorithm<StoredValuesPointsToAlgorithm>
        ::handleRelation<RT>(Src, Dst);
  }
}
}

#endif
