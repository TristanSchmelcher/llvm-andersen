//===- LazyAndersenMetaAnalysisStep.h - analysis classes ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for meta analysis--i.e., analysis of other analysis
// results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENMETAANALYSISSTEP_H
#define LAZYANDERSENMETAANALYSISSTEP_H

#include "LazyAndersenAnalysisResultEntry.h"
#include "LazyAndersenIterativeAnalysisStep.h"
#include "LazyAndersenSavedIterator.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  class MetaAnalysisStep : public IterativeAnalysisStep {
    SavedIterator<AnalysisResultEntry> Iterator;

  public:
    explicit MetaAnalysisStep(AnalysisResult *AR);
    ~MetaAnalysisStep();

    virtual void run();
  };
}
}

#endif
