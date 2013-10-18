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
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResultEntryBase : public ilist_node<AnalysisResultEntryBase> {
  public:
    enum EntryType {
      VALUE_INFO_ENTRY,
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
  struct ilist_traits<lazyandersen::AnalysisResultEntryBase>
    : public ilist_nextprev_traits<lazyandersen::AnalysisResultEntryBase>,
      public ilist_ghostly_sentinel_traits<lazyandersen::AnalysisResultEntryBase>,
      public ilist_node_traits<lazyandersen::AnalysisResultEntryBase> {};
}

#endif
