//===- LazyAndersenIntrusiveListTraits.h - ilist utilities ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines intrusive list traits for the LazyAndersen algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINTRUSIVELISTTRAITS_H
#define LAZYANDERSENINTRUSIVELISTTRAITS_H

#include "llvm/ADT/ilist.h"

namespace llvm {
namespace lazyandersen {
  // Traits for a intrusive list with a "ghostly sentinel" whose nodes maintain
  // a back-pointer to the list itself. NodeTy must inherit from
  // intrusive_list_node.
  template<typename NodeTy>
  struct IntrusiveListTraits : public ilist_default_traits<NodeTy> {
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
  inline void IntrusiveListTraits<NodeTy>::destroySentinel(
      NodeTy *) const {}

  template<typename NodeTy>
  inline NodeTy *IntrusiveListTraits<NodeTy>::provideInitialHead()
      const {
    return static_cast<const ilist_traits<NodeTy> *>(this)->createSentinel();
  }

  template<typename NodeTy>
  inline NodeTy *IntrusiveListTraits<NodeTy>::ensureHead(NodeTy *)
      const {
    return static_cast<const ilist_traits<NodeTy> *>(this)->createSentinel();
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

#define INTRUSIVE_LIST_TRAITS(TYPE) \
  template<> \
  struct ilist_traits<TYPE> \
      : public lazyandersen::IntrusiveListTraits<TYPE> { \
  private: \
    /* Have to duplicate the Sentinel logic for each specialization because the
       ilist_half_node constructor is protected and only friends with
       ilist_traits. */ \
    mutable ilist_half_node<TYPE> Sentinel; \
\
  public: \
    TYPE *createSentinel() const { \
      return static_cast<TYPE *>(&Sentinel); \
    } \
  }

}
}

#endif
