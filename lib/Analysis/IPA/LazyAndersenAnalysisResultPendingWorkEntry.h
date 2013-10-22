//===- LazyAndersenAnalysisResultPendingWorkEntry.h - algorithm classes ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for entries in analysis results that represent
// pending work.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTPENDINGWORKENTRY_H
#define LAZYANDERSENANALYSISRESULTPENDINGWORKENTRY_H

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultEntry.h"
#include "LazyAndersenAnalysisResultEntryBase.h"
#include "LazyAndersenGraphNode.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResultPendingWorkEntry :
      public AnalysisResultEntry<AnalysisResultEntryBase::PENDING_WORK_ENTRY>,
      public GraphNode {
  public:
    using AnalysisResultEntry<AnalysisResultEntryBase::PENDING_WORK_ENTRY>
        ::classof;

    AnalysisResultPendingWorkEntry();
    virtual ~AnalysisResultPendingWorkEntry();
    virtual AnalysisResult::EnumerationResult enumerate(
        AnalysisResult::Enumerator::Context *Ctx) = 0;
    virtual bool isNodeHidden() const;
  };
}
}

#endif
