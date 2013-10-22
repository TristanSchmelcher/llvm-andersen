//===- LazyAndersenRelationsAnalysisStepBase.h - analysis classes ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a base type for relations analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONSANALYSISSTEPBASE_H
#define LAZYANDERSENRELATIONSANALYSISSTEPBASE_H

#include "LazyAndersenAnalysisResultPendingWorkEntry.h"
#include "LazyAndersenHalfRelationBaseList.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  class RelationsAnalysisStepBase : public AnalysisResultPendingWorkEntry {
  protected:
    HalfRelationBaseList *List;
    HalfRelationBaseList::iterator i;

  public:
    explicit RelationsAnalysisStepBase(HalfRelationBaseList *List);
    virtual AnalysisResult::EnumerationResult enumerate(
        AnalysisResult::Enumerator::Context *Ctx);

  private:
    virtual AnalysisResult *analyzeHalfRelation(HalfRelationBase *HR) = 0;
  };
}
}

#endif
