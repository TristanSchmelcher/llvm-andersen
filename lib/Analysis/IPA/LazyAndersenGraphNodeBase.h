//===- LazyAndersenGraphNodeBase.h - graph viewing ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a base type for polymorphic graph nodes.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENGRAPHNODEBASE_H
#define LAZYANDERSENGRAPHNODEBASE_H

#include "llvm/ADT/StringRef.h"

#include <list>

namespace llvm {
namespace lazyandersen {
  class GraphNodeBase;

  struct GraphEdge {
    GraphEdge() : Dst(0) {}
    GraphEdge(const GraphNodeBase *Dst, StringRef Label)
      : Dst(Dst), Label(Label.str()) {}

    const GraphNodeBase *Dst;
    std::string Label;
  };

  class GraphNodeBase {
  public:
    enum Type {
      ROOT,
      VALUE_INFO,
      ANALYSIS_RESULT,
      ANALYSIS_RESULT_PENDING_WORK_ENTRY
    };

    virtual Type getType() const = 0;
    virtual std::list<GraphEdge> getOutgoingEdges() const = 0;

  protected:
    ~GraphNodeBase() {}
  };
}
}

#endif
