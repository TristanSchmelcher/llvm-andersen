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

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultPendingWorkEntry.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  class MetaAnalysisStep : public AnalysisResultPendingWorkEntry {
    AnalysisResult::Enumerator E;

  public:
    explicit MetaAnalysisStep(AnalysisResult *AR);
    virtual ~MetaAnalysisStep();
    virtual GraphEdgeDeque getOutgoingEdges() const;

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) = 0;

    virtual AnalysisResult::EnumerationResult enumerate(
        AnalysisResult::Enumerator::Context *Ctx);
  };
}
}

#endif
