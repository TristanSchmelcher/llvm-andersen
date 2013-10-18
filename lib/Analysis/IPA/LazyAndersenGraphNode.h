//===- LazyAndersenGraphNode.h - graph viewing ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for polymorphic graph nodes.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENGRAPHNODE_H
#define LAZYANDERSENGRAPHNODE_H

#include "llvm/ADT/StringRef.h"

#include <deque>

namespace llvm {
namespace lazyandersen {
  class GraphNode;

  struct GraphEdge {
    GraphEdge() : Dst(0) {}
    GraphEdge(const GraphNode *Dst, StringRef Label)
      : Dst(Dst), Label(Label.str()) {}

    const GraphNode *Dst;
    std::string Label;
  };

  typedef std::deque<GraphEdge> GraphEdgeDeque;

  class GraphNode {
  public:
    virtual GraphEdgeDeque getOutgoingEdges() const = 0;
    virtual std::string getNodeLabel() const = 0;
    virtual bool isNodeHidden() const = 0;

  protected:
    ~GraphNode() {}
  };
}
}

#endif