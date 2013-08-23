//===- LazyAndersenAnalysisResultEntryBase.h - algorithm classes ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a base type for entries in analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTENTRYBASE_H
#define LAZYANDERSENANALYSISRESULTENTRYBASE_H

#include "LazyAndersenGraphNode.h"
#include "LazyAndersenIntrusiveListWithSavedIteratorSupportNode.h"
#include "LazyAndersenIntrusiveListWithSavedIteratorSupportTraits.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResultEntryBase :
      public IntrusiveListWithSavedIteratorSupportNode<
          AnalysisResultEntryBase> {
    friend struct ilist_nextprev_traits<AnalysisResultEntryBase>;
    friend struct ilist_node_traits<AnalysisResultEntryBase>;
    friend struct IntrusiveListTraits<AnalysisResultEntryBase>;
    friend struct IntrusiveListWithSavedIteratorSupportTraits<
        AnalysisResultEntryBase>;

  public:
    enum EntryType {
      VALUE_INFO_ENTRY,
      RECURSIVE_ENTRY,
      PENDING_WORK_ENTRY
    };

    AnalysisResultEntryBase();
    virtual ~AnalysisResultEntryBase();
    virtual EntryType getEntryType() const = 0;
    virtual const GraphNode *getGraphNode() const;
  };
}
}

namespace llvm {
  template<>
  struct ilist_traits<lazyandersen::AnalysisResultEntryBase> :
      public lazyandersen::IntrusiveListWithSavedIteratorSupportTraits<
          lazyandersen::AnalysisResultEntryBase> {
  };
}

#endif
