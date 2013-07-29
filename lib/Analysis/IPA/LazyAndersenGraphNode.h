//===- LazyAndersenGraphNode.h - graph viewing ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a template type for polymorphic graph nodes.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENGRAPHNODE_H
#define LAZYANDERSENGRAPHNODE_H

#include "LazyAndersenGraphNodeBase.h"

namespace llvm {
namespace lazyandersen {
  template<GraphNodeBase::Type Ty>
  class GraphNode : public GraphNodeBase {
  public:
    virtual GraphNodeBase::Type getType() const {
      return Ty;
    }

  protected:
    static bool classof(const GraphNodeBase *Base) {
      return Base->getType() == Ty;
    }
  };
}
}

#endif
