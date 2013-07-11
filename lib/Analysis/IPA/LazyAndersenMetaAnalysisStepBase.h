//===- LazyAndersenMetaAnalysisStepBase.h - analysis classes --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a base type for meta analysis--i.e., analysis of other
// analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENMETAANALYSISSTEPBASE_H
#define LAZYANDERSENMETAANALYSISSTEPBASE_H

#include "LazyAndersenAnalysisResultEntry.h"
#include "LazyAndersenIterativeAnalysisStep.h"
#include "LazyAndersenSavedIterator.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  class MetaAnalysisStepBase : public IterativeAnalysisStep {
    SavedIterator<AnalysisResultEntry> SI;

  public:
    explicit MetaAnalysisStepBase(AnalysisResult *AR);
    virtual ~MetaAnalysisStepBase();

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) = 0;
    virtual AnalysisResult *analyzeRecursive(AnalysisResult *AR) = 0;

    virtual void run();
  };
}
}

#endif
