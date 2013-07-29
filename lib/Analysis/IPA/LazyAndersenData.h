//===- LazyAndersenData.h - all analysis data -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type holding all analysis information.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENDATA_H
#define LAZYANDERSENDATA_H

#include "LazyAndersenGraphNode.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
  class LazyAndersenData :
    public lazyandersen::GraphNode<lazyandersen::GraphNodeBase::ROOT> {
  public:
    using lazyandersen::GraphNode<GraphNodeBase::ROOT>::classof;

    lazyandersen::ValueInfo::Map ValueInfos;

    virtual std::list<lazyandersen::GraphEdge> getOutgoingEdges() const;

    virtual ~LazyAndersenData() {}
  };
}

#endif
