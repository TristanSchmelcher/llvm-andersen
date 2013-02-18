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

#include "LazyAndersenIntrusiveListWithSavedIteratorSupportNode.h"
#include "LazyAndersenIntrusiveListWithSavedIteratorSupportTraits.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResultEntry :
      protected IntrusiveListWithSavedIteratorSupportNode<AnalysisResultEntry> {
    friend struct ilist_nextprev_traits<AnalysisResultEntry>;
    friend struct ilist_node_traits<AnalysisResultEntry>;
    friend struct IntrusiveListTraits<AnalysisResultEntry>;
    friend struct IntrusiveListWithSavedIteratorSupportTraits<
        AnalysisResultEntry>;

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
  template<>
  struct ilist_traits<lazyandersen::AnalysisResultEntry> :
      public lazyandersen::IntrusiveListWithSavedIteratorSupportTraits<
          lazyandersen::AnalysisResultEntry> {};
}

#endif
