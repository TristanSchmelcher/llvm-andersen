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

#include "LazyAndersenAnalysisResultEntry.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResultPendingWorkEntry : public AnalysisResultEntry {
  public:
    AnalysisResultPendingWorkEntry();
    virtual ~AnalysisResultPendingWorkEntry();
    virtual EntryType getEntryType() const;
    virtual void run() = 0;

    static bool classof(const AnalysisResultEntry *Base) {
      return Base->getEntryType() == PENDING_WORK_ENTRY;
    }
  };
}
}

#endif
