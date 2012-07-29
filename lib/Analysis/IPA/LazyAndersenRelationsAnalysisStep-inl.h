//===- LazyAndersenRelationsAnalysisStep-inl.h - analysis classes ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for a template type for relations
// analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONSANALYSISSTEP_INL_H
#define LAZYANDERSENRELATIONSANALYSISSTEP_INL_H

#include "LazyAndersenRelationsAnalysisStep.h"

#include "LazyAndersenHalfRelation-inl.h"
#include "LazyAndersenHalfRelationBase-inl.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  template<RelationDirection Direction, typename StepTy>
  RelationsAnalysisStep<Direction, StepTy>::RelationsAnalysisStep(
      ValueInfo *VI)
    : RelationsAnalysisStepBase(VI->getRelations<Direction>()) {}

  template<RelationDirection Direction, typename StepTy>
  AnalysisResult *RelationsAnalysisStep<Direction, StepTy>::
      analyzeHalfRelation(HalfRelationBase *HR) {
    return static_cast<StepTy *>(this)->analyzeRelation(
        HR->as<Direction>()->getRelation());
  }
}
}

#endif
