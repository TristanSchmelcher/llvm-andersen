//===- LazyAndersenIterativeAnalysisStep.h - analysis classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for iterative analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENITERATIVEANALYSISSTEP_H
#define LAZYANDERSENITERATIVEANALYSISSTEP_H

#include "LazyAndersenAnalysisResultEntryBaseList.h"
#include "LazyAndersenAnalysisResultPendingWorkEntry.h"

namespace llvm {
namespace lazyandersen {
  class IterativeAnalysisStep : public AnalysisResultPendingWorkEntry {
  protected:
    // Push the next iteration onto the entry list and defer to it.
    AnalysisResult::EnumerationResult emit(
        AnalysisResult *Owner,
        AnalysisResultEntryBaseList::iterator *j,
        int Depth,
        AnalysisResult *Next);
  };
}
}

#endif
