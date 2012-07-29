//===- LazyAndersenRelationsAnalysisStep.h - analysis classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a template type for relations analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONSANALYSISSTEP_H
#define LAZYANDERSENRELATIONSANALYSISSTEP_H

#include "LazyAndersenRelationDirection.h"
#include "LazyAndersenRelationsAnalysisStepBase.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  template<RelationDirection Direction, typename StepTy>
  class RelationsAnalysisStep : public RelationsAnalysisStepBase {
  public:
    explicit RelationsAnalysisStep(ValueInfo *VI);

  private:
    virtual AnalysisResult *analyzeHalfRelation(HalfRelationBase *HR);
  };
}
}

#endif
