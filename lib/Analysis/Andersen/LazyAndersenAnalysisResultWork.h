//===- LazyAndersenAnalysisResultWork.h - algorithm classes ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for pending work for analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTWORK_H
#define LAZYANDERSENANALYSISRESULTWORK_H

#include "LazyAndersenGraphNode.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"

namespace llvm {
namespace lazyandersen {
  class EnumerationContext;
  class EnumerationResult;

  class AnalysisResultWork :
      public ilist_node<AnalysisResultWork>,
      public GraphNode {
  public:
    AnalysisResultWork();
    virtual ~AnalysisResultWork();
    virtual bool isNodeHidden() const;

    virtual EnumerationResult enumerate(EnumerationContext *Ctx) = 0;
  };

  typedef iplist<AnalysisResultWork> AnalysisResultWorkList;
}
}

namespace llvm {
  template<>
  struct ilist_traits<lazyandersen::AnalysisResultWork>
    : public ilist_nextprev_traits<lazyandersen::AnalysisResultWork>,
      public ilist_ghostly_sentinel_traits<lazyandersen::AnalysisResultWork>,
      public ilist_node_traits<lazyandersen::AnalysisResultWork> {};
}

#endif
