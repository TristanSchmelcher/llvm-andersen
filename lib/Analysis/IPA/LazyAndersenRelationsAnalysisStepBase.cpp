//===- LazyAndersenRelationsAnalysisStepBase.cpp - analysis classes -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a base type for relations analysis.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenRelationsAnalysisStepBase.h"

#include "LazyAndersenRecursiveAnalysisStep.h"

using namespace llvm;
using namespace llvm::lazyandersen;

RelationsAnalysisStepBase::RelationsAnalysisStepBase(
    HalfRelationBaseList *List)
  : List(List), i(List->begin()) {}

RelationsAnalysisStepBase::Result RelationsAnalysisStepBase::run() {
  while (i != List->end()) {
    HalfRelationBase *HR = &*i;
    ++i;
    AnalysisResult *LR = analyzeHalfRelation(HR);
    if (LR) {
      return nextIteration(new RecursiveAnalysisStep(LR));
    }
    // Else nothing to do for this relation; keep going.
  }
  // No relations left to analyze.
  return done();
}
