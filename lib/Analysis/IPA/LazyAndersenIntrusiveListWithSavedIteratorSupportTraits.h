//===- LazyAndersenIntrusiveListWithSavedIteratorsTraits.h ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines intrusive list traits for node types inheriting from
// IntrusiveListWithSavedIteratorSupportNode.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINTRUSIVELISTWITHSAVEDITERATORSUPPORTTRAITS_H
#define LAZYANDERSENINTRUSIVELISTWITHSAVEDITERATORSUPPORTTRAITS_H

#include "LazyAndersenIntrusiveListTraits.h"

namespace llvm {
namespace lazyandersen {
  // NodeTy must inherit from IntrusiveListWithSavedIteratorSupportNode.
  template<typename NodeTy>
  struct IntrusiveListWithSavedIteratorSupportTraits :
      public IntrusiveListTraits<NodeTy> {
  private:
    // Never defined (unsupported operation).
    void transferNodesFromList(
        ilist_node_traits<NodeTy> &that,
        ilist_iterator<NodeTy> first,
        ilist_iterator<NodeTy> last);

  public:
    void removeNodeFromList(NodeTy *Node);
  };
}
}

#endif
