//===- LazyAndersenMetaAnalysisStep.h - analysis classes ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a template type for meta analysis--i.e., analysis of other
// analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENMETAANALYSISSTEP_H
#define LAZYANDERSENMETAANALYSISSTEP_H

#include "LazyAndersenMetaAnalysisStepBase.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  template<AnalysisResultAlgorithmId Id>
  class MetaAnalysisStep : public MetaAnalysisStepBase {
  public:
    explicit MetaAnalysisStep(AnalysisResult *AR) : MetaAnalysisStepBase(AR) {}

    virtual AnalysisResult *analyzeRecursive(AnalysisResult *AR) {
      return AR->getAlgorithmResult<Id>();
    }
  };
}
}

#endif
