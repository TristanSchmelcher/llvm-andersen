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
  struct IntrusiveListTraits : public ilist_default_traits<NodeTy> {
  private:
    // In principle, this could be ilist_half_node like with other ghostly
    // sentinel ilists, but that's only an accessible base to friends of
    // ilist_node.
    // Also, the ilist_node constructor is protected, so we must derive from it
    // to raise it to public.
    class SentinelTy : public ilist_node<NodeTy> {};
    mutable SentinelTy Sentinel;

  public:
    NodeTy *createSentinel() const;
    void destroySentinel(NodeTy *) const;
    NodeTy *provideInitialHead() const;
    NodeTy *ensureHead(NodeTy *) const;
    static void noteHead(NodeTy *, NodeTy *);
    void addNodeToList(NodeTy *Node);
    void removeNodeFromList(NodeTy *Node);
    void transferNodesFromList(
        ilist_node_traits<NodeTy> &that,
        ilist_iterator<NodeTy> first,
        ilist_iterator<NodeTy> last);
  };

  template<typename NodeTy>
  inline NodeTy *IntrusiveListTraits<NodeTy>::createSentinel() const {
    return static_cast<NodeTy *>(
        static_cast<ilist_node<NodeTy> *>(&Sentinel));
  }

  template<typename NodeTy>
  inline void IntrusiveListTraits<NodeTy>::destroySentinel(
      NodeTy *) const {}

  template<typename NodeTy>
  inline NodeTy *IntrusiveListTraits<NodeTy>::provideInitialHead()
      const {
    return createSentinel();
  }

  template<typename NodeTy>
  inline NodeTy *IntrusiveListTraits<NodeTy>::ensureHead(NodeTy *)
      const {
    return createSentinel();
  }

  template<typename NodeTy>
  inline void IntrusiveListTraits<NodeTy>::noteHead(NodeTy *,
      NodeTy *) {}

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
