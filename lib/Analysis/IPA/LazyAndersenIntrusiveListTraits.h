//===- LazyAndersenIntrusiveListTraits.h - ilist utilities ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines intrusive list traits for node types inheriting from
// IntrusiveListNode.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINTRUSIVELISTTRAITS_H
#define LAZYANDERSENINTRUSIVELISTTRAITS_H

#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"

namespace llvm {
namespace lazyandersen {
  // NodeTy must inherit from IntrusiveListNode.
  template<typename NodeTy>
  struct IntrusiveListTraits
    : public ilist_nextprev_traits<NodeTy>,
      public ilist_ghostly_sentinel_traits<NodeTy>,
      public ilist_node_traits<NodeTy> {
    void addNodeToList(NodeTy *Node);
    void removeNodeFromList(NodeTy *Node);
    void transferNodesFromList(
        ilist_node_traits<NodeTy> &that,
        ilist_iterator<NodeTy> first,
        ilist_iterator<NodeTy> last);
  };

  template<typename NodeTy>
  inline void IntrusiveListTraits<NodeTy>::addNodeToList(
      NodeTy *Node) {
    Node->added(static_cast<ilist<NodeTy> *>(this));
  }

  template<typename NodeTy>
  inline void IntrusiveListTraits<NodeTy>::removeNodeFromList(
      NodeTy *Node) {
    Node->removed(static_cast<ilist<NodeTy> *>(this));
  }

  template<typename NodeTy>
  void IntrusiveListTraits<NodeTy>::transferNodesFromList(
      ilist_node_traits<NodeTy> &that,
      ilist_iterator<NodeTy> first,
      ilist_iterator<NodeTy> last) {
    for (ilist_iterator<NodeTy> i = first; i != last; ++i) {
      i->transferred(static_cast<ilist<NodeTy> *>(this),
          static_cast<ilist<NodeTy> *>(&that));
    }
  }
}
}

#endif
