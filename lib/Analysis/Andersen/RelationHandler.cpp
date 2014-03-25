//===- RelationHandler.cpp ------------------------------------------------===//
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

#include "RelationHandler.h"

#include "ActualParametersPointsToAlgorithm.h"
#include "ActualReturnValuePointsToAlgorithm.h"
#include "FormalParametersReversePointsToAlgorithm.h"
#include "FormalReturnValueReversePointsToAlgorithm.h"
#include "LoadedValuesReversePointsToAlgorithm.h"
#include "ModifiedValuesPointsToAlgorithm.h"
#include "PointsToAlgorithm.h"
#include "ReferencedValuesPointsToAlgorithm.h"
#include "ReversePointsToAlgorithm.h"
#include "StoredValuesPointsToAlgorithm.h"
#include "TraversalAlgorithm.h"
#include "ValueInfo.h"

#include <cassert>

namespace llvm {
namespace andersen_internal {

namespace {

template<typename AlgorithmTy>
struct ForAlgorithm {
  template<BinaryRelation RT>
  static void handleRelation(ValueInfo *Src, ValueInfo *Dst) {}

  template<TrinaryRelation RT>
  static void handleRelation(Data *D, ValueInfo *Src, ValueInfo *Dst,
      const Instruction *CallSite) {}
};

// ActualParametersPointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<ActualParametersPointsToAlgorithm>
    ::handleRelation<ARGUMENT_TO_CALLEE>(Data *D, ValueInfo *Src,
        ValueInfo *Dst, const Instruction *CallSite) {
  // Add work to push callsite annotation.
  Dst->addInstructionAnalysisWork<ActualParametersPointsToAlgorithm,
      PointsToAlgorithm>(Src);
}

// ActualReturnValuePointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<ActualReturnValuePointsToAlgorithm>
    ::handleRelation<RETURNED_TO_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do; callsite is popped by RETURNED_FROM_CALLEE.
  Dst->addInstructionAnalysisWork<ActualReturnValuePointsToAlgorithm,
      PointsToAlgorithm>(Src);
}

// FormalParametersReversePointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<FormalParametersReversePointsToAlgorithm>
    ::handleRelation<ARGUMENT_FROM_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do; callsite is popped by ARGUMENT_TO_CALLEE.
  Dst->addInstructionAnalysisWork<FormalParametersReversePointsToAlgorithm,
      ReversePointsToAlgorithm>(Src);
}

// FormalReturnValueReversePointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<FormalReturnValueReversePointsToAlgorithm>
    ::handleRelation<RETURNED_FROM_CALLEE>(Data *D, ValueInfo *Src,
        ValueInfo *Dst, const Instruction *CallSite) {
  // Add work to push callsite annotation.
  Dst->addInstructionAnalysisWork<FormalReturnValueReversePointsToAlgorithm,
      ReversePointsToAlgorithm>(Src);
}

// LoadedValuesReversePointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<LoadedValuesReversePointsToAlgorithm>
    ::handleRelation<LOADED_FROM>(ValueInfo *Src, ValueInfo *Dst) {
  // Add work to drop callsite annotations.
  Dst->addInstructionAnalysisWork<LoadedValuesReversePointsToAlgorithm,
      ReversePointsToAlgorithm>(Src);
}

// ModifiedValuesPointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<ModifiedValuesPointsToAlgorithm>
    ::handleRelation<CALLS>(Data *D, ValueInfo *Src, ValueInfo *Dst,
        const Instruction *CallSite) {
  // Add work to pop callsite.
  Src->addInstructionAnalysisWork<
      ModifiedValuesPointsToAlgorithm,
      TwoHopTraversal<PointsToAlgorithm,
                      ModifiedValuesPointsToAlgorithm>::Algorithm>(Dst);
}

template<>
template<>
inline void ForAlgorithm<ModifiedValuesPointsToAlgorithm>
    ::handleRelation<WRITES_TO>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do.
  Src->addInstructionAnalysisWork<ModifiedValuesPointsToAlgorithm,
      PointsToAlgorithm>(Dst);
}

// PointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<PointsToAlgorithm>
    ::handleRelation<ARGUMENT_FROM_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do; callsite has been pushed by ARGUMENT_TO_CALLEE.
  Src->addInstructionAnalysisWork<
      PointsToAlgorithm,
      TwoHopTraversal<ReversePointsToAlgorithm,
                      ActualParametersPointsToAlgorithm>::Algorithm>(Dst);
}

template<>
template<>
inline void ForAlgorithm<PointsToAlgorithm>
    ::handleRelation<DEPENDS_ON>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do.
  Src->addInstructionAnalysisWork<PointsToAlgorithm,
      PointsToAlgorithm>(Dst);
}

template<>
template<>
inline void ForAlgorithm<PointsToAlgorithm>
    ::handleRelation<LOADED_FROM>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do; callsites have been dropped by STORED_TO.
  Src->addInstructionAnalysisWork<
      PointsToAlgorithm,
      ThreeHopTraversal<PointsToAlgorithm,
                        ReversePointsToAlgorithm,
                        StoredValuesPointsToAlgorithm>::Algorithm>(Dst);
}

template<>
template<>
inline void ForAlgorithm<PointsToAlgorithm>
    ::handleRelation<RETURNED_FROM_CALLEE>(Data *D, ValueInfo *Src,
        ValueInfo *Dst, const Instruction *CallSite) {
  // Add work to pop callsite annotation.
  Src->addInstructionAnalysisWork<
      PointsToAlgorithm,
      TwoHopTraversal<PointsToAlgorithm,
                      ActualReturnValuePointsToAlgorithm>::Algorithm>(Dst);
}

// ReferencedValuesPointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<ReferencedValuesPointsToAlgorithm>
    ::handleRelation<CALLS>(Data *D, ValueInfo *Src, ValueInfo *Dst,
        const Instruction *CallSite) {
  // Add work to pop callsite.
  Src->addInstructionAnalysisWork<
      ReferencedValuesPointsToAlgorithm,
      TwoHopTraversal<PointsToAlgorithm,
                      ReferencedValuesPointsToAlgorithm>::Algorithm>(Dst);
}

template<>
template<>
inline void ForAlgorithm<ReferencedValuesPointsToAlgorithm>
    ::handleRelation<READS_FROM>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do.
  Src->addInstructionAnalysisWork<ReferencedValuesPointsToAlgorithm,
      PointsToAlgorithm>(Dst);
}

// ReversePointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<ReversePointsToAlgorithm>
    ::handleRelation<ARGUMENT_TO_CALLEE>(Data *D, ValueInfo *Src,
        ValueInfo *Dst, const Instruction *CallSite) {
  // Add work to pop callsite annotation.
  Src->addInstructionAnalysisWork<
      ReversePointsToAlgorithm,
      TwoHopTraversal<PointsToAlgorithm,
                      FormalParametersReversePointsToAlgorithm>::Algorithm>(
          Dst);
}

template<>
template<>
inline void ForAlgorithm<ReversePointsToAlgorithm>
    ::handleRelation<DEPENDS_ON>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do.
  Dst->addInstructionAnalysisWork<ReversePointsToAlgorithm,
      ReversePointsToAlgorithm>(Src);
}

template<>
template<>
inline void ForAlgorithm<ReversePointsToAlgorithm>
    ::handleRelation<RETURNED_TO_CALLER>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do; callsite is pushed by RETURNED_FROM_CALLEE.
  Src->addInstructionAnalysisWork<
      ReversePointsToAlgorithm,
      TwoHopTraversal<
          ReversePointsToAlgorithm,
          FormalReturnValueReversePointsToAlgorithm>::Algorithm>(Dst);
}

template<>
template<>
inline void ForAlgorithm<ReversePointsToAlgorithm>
    ::handleRelation<STORED_TO>(ValueInfo *Src, ValueInfo *Dst) {
  // Nothing special to do; callsites have been dropped by LOADED_FROM.
  Src->addInstructionAnalysisWork<
      ReversePointsToAlgorithm,
      ThreeHopTraversal<
          PointsToAlgorithm,
          ReversePointsToAlgorithm,
          LoadedValuesReversePointsToAlgorithm>::Algorithm>(Dst);
}

// StoredValuesPointsToAlgorithm
template<>
template<>
inline void ForAlgorithm<StoredValuesPointsToAlgorithm>
    ::handleRelation<STORED_TO>(ValueInfo *Src, ValueInfo *Dst) {
  // Add work to drop callsite annotations.
  Dst->addInstructionAnalysisWork<StoredValuesPointsToAlgorithm,
      PointsToAlgorithm>(Src);
}

}

// Now put it all together.
template<BinaryRelation RT>
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
  ForAlgorithm<ModifiedValuesPointsToAlgorithm>
      ::handleRelation<RT>(Src, Dst);
  ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<RT>(Src, Dst);
  ForAlgorithm<ReferencedValuesPointsToAlgorithm>
      ::handleRelation<RT>(Src, Dst);
  ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<RT>(Src, Dst);
  ForAlgorithm<StoredValuesPointsToAlgorithm>
      ::handleRelation<RT>(Src, Dst);
}

// Now put it all together.
template<TrinaryRelation RT>
void RelationHandler::handleRelation(ValueInfo *Src, ValueInfo *Dst,
    const Instruction *CallSite) {
  assert(Src);
  assert(Dst);
  // Dispatch to every relation-based algorithm.
  ForAlgorithm<ActualParametersPointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<ActualReturnValuePointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<FormalParametersReversePointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<FormalReturnValueReversePointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<LoadedValuesReversePointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<ModifiedValuesPointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<PointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<ReferencedValuesPointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<ReversePointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
  ForAlgorithm<StoredValuesPointsToAlgorithm>
      ::handleRelation<RT>(D, Src, Dst, CallSite);
}

// Compile for each relation.
template void RelationHandler::handleRelation<ARGUMENT_FROM_CALLER>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<ARGUMENT_TO_CALLEE>(
    ValueInfo *Src, ValueInfo *Dst, const Instruction *CallSite);
template void RelationHandler::handleRelation<DEPENDS_ON>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<LOADED_FROM>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<RETURNED_FROM_CALLEE>(
    ValueInfo *Src, ValueInfo *Dst, const Instruction *CallSite);
template void RelationHandler::handleRelation<RETURNED_TO_CALLER>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<STORED_TO>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<CALLS>(
    ValueInfo *Src, ValueInfo *Dst, const Instruction *CallSite);
template void RelationHandler::handleRelation<READS_FROM>(
    ValueInfo *Src, ValueInfo *Dst);
template void RelationHandler::handleRelation<WRITES_TO>(
    ValueInfo *Src, ValueInfo *Dst);

}
}
