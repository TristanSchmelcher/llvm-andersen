//===- LazyAndersenAnalysisResultEntry.h - algorithm classes --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for entries in analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTENTRY_H
#define LAZYANDERSENANALYSISRESULTENTRY_H

#include "LazyAndersenIntrusiveListNode.h"
#include "LazyAndersenIntrusiveListTraits.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResultEntry :
      protected IntrusiveListNode<AnalysisResultEntry> {
    friend struct ilist_nextprev_traits<AnalysisResultEntry>;
    friend struct ilist_node_traits<AnalysisResultEntry>;
    friend struct IntrusiveListTraits<AnalysisResultEntry>;
    friend struct ilist_traits<AnalysisResultEntry>;

  public:
    enum EntryType {
      VALUE_INFO_ENTRY,
      RECURSIVE_ENTRY,
      PENDING_WORK_ENTRY
    };

    AnalysisResultEntry();
    virtual ~AnalysisResultEntry();
    virtual EntryType getEntryType() const = 0;
  };
}
}

namespace llvm {
  INTRUSIVE_LIST_TRAITS(lazyandersen::AnalysisResultEntry)
}

#endif
