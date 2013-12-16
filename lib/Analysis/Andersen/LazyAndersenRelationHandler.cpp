//===- LazyAndersenRelationHandler.cpp ------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a class for translating the relations produced by
// instruction analysis into graph traversal steps.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenRelationHandler.h"

#include "LazyAndersenActualParametersPointsToAlgorithm.h"
#include "LazyAndersenActualReturnValuePointsToAlgorithm.h"
#include "LazyAndersenFormalParametersReversePointsToAlgorithm.h"
#include "LazyAndersenFormalReturnValueReversePointsToAlgorithm.h"
#include "LazyAndersenLoadedValuesReversePointsToAlgorithm.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenReversePointsToAlgorithm.h"
#include "LazyAndersenStoredValuesPointsToAlgorithm.h"
#include "LazyAndersenTraversalAlgorithm.h"
#include "LazyAndersenValueInfo.h"

#include <cassert>

using namespace llvm;
using namespace llvm::andersen_internal;

namespace {
  template<typename AlgorithmTy>
  struct ForAlgorithm;

  // ActualParametersPointsToAlgorithm
  template<>
  struct ForAlgorithm<ActualParametersPointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void ForAlgorithm<ActualParametersPointsToAlgorithm>
      ::handleRelation<ARGUMENT_TO_CALLEE>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<ActualParametersPointsToAlgorithm,
        PointsToAlgorithm>(Src);
  }

  // ActualReturnValuePointsToAlgorithm
  template<>
  struct ForAlgorithm<ActualReturnValuePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void ForAlgorithm<ActualReturnValuePointsToAlgorithm>
      ::handleRelation<RETURNED_TO_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<ActualReturnValuePointsToAlgorithm,
        PointsToAlgorithm>(Src);
  }

  // FormalParametersReversePointsToAlgorithm
  template<>
  struct ForAlgorithm<FormalParametersReversePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void ForAlgorithm<FormalParametersReversePointsToAlgorithm>
      ::handleRelation<ARGUMENT_FROM_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<FormalParametersReversePointsToAlgorithm,
        ReversePointsToAlgorithm>(Src);
  }

  // FormalReturnValueReversePointsToAlgorithm
  template<>
  struct ForAlgorithm<FormalReturnValueReversePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void ForAlgorithm<FormalReturnValueReversePointsToAlgorithm>
      ::handleRelation<RETURNED_FROM_CALLEE>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<FormalReturnValueReversePointsToAlgorithm,
        ReversePointsToAlgorithm>(Src);
  }

  // LoadedValuesReversePointsToAlgorithm
  template<>
  struct ForAlgorithm<LoadedValuesReversePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void ForAlgorithm<LoadedValuesReversePointsToAlgorithm>
      ::handleRelation<LOADED_FROM>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<LoadedValuesReversePointsToAlgorithm,
        ReversePointsToAlgorithm>(Src);
  }

  // PointsToAlgorithm
  template<>
  struct ForAlgorithm<PointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<ARGUMENT_FROM_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        PointsToAlgorithm,
        TwoHopTraversal<ReversePointsToAlgorithm,
                        ActualParametersPointsToAlgorithm>::Algorithm>(Dst);
  }

  template<>
  inline void ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<DEPENDS_ON>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<PointsToAlgorithm,
        PointsToAlgorithm>(Dst);
  }

  template<>
  inline void ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<LOADED_FROM>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        PointsToAlgorithm,
        ThreeHopTraversal<PointsToAlgorithm,
                          ReversePointsToAlgorithm,
                          StoredValuesPointsToAlgorithm>::Algorithm>(Dst);
  }

  template<>
  inline void ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<RETURNED_FROM_CALLEE>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        PointsToAlgorithm,
        TwoHopTraversal<PointsToAlgorithm,
                        ActualReturnValuePointsToAlgorithm>::Algorithm>(Dst);
  }

  // ReversePointsToAlgorithm
  template<>
  struct ForAlgorithm<ReversePointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<ARGUMENT_TO_CALLEE>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        ReversePointsToAlgorithm,
        TwoHopTraversal<PointsToAlgorithm,
                        FormalParametersReversePointsToAlgorithm>::Algorithm>(
            Dst);
  }

  template<>
  inline void ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<DEPENDS_ON>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<ReversePointsToAlgorithm,
        ReversePointsToAlgorithm>(Src);
  }

  template<>
  inline void ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<RETURNED_TO_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
    Src->addInstructionAnalysisWork<
        ReversePointsToAlgorithm,
        TwoHopTraversal<
            ReversePointsToAlgorithm,
            FormalReturnValueReversePointsToAlgorithm>::Algorithm>(Dst);
  }

  template<>
  inline void ForAlgorithm<ReversePointsToAlgorithm>
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
  struct ForAlgorithm<StoredValuesPointsToAlgorithm> {
    template<RelationType RT>
    static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}
  };

  template<>
  inline void ForAlgorithm<StoredValuesPointsToAlgorithm>
      ::handleRelation<STORED_TO>(ValueInfo *Src, ValueInfo *Dst) {
    Dst->addInstructionAnalysisWork<StoredValuesPointsToAlgorithm,
        PointsToAlgorithm>(Src);
  }
}

// Now put it all together.
template<RelationType RT>
void RelationHandler::handleRelation(ValueInfo *Src, ValueInfo *Dst) {
  assert(Src);
  assert(Dst);
  // Dispatch to every relation-based algorithm.
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

// Compile for each relation.
template void RelationHandler::handleRelation<ARGUMENT_FROM_CALLER>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<ARGUMENT_TO_CALLEE>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<DEPENDS_ON>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<LOADED_FROM>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<RETURNED_FROM_CALLEE>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<RETURNED_TO_CALLER>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<STORED_TO>(
    ValueInfo *Src, ValueInfo *Dst);
