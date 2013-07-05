//===- LazyAndersenIntrusiveListWithSavedIteratorSupportNode.h ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an intrusive list node type that allows "saving" iterators.
// Saved iterators are never invalidated. Removing a node causes any saved
// iterators for it to advance to the next node.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINTRUSIVELISTWITHSAVEDITERATORSUPPORTNODE_H
#define LAZYANDERSENINTRUSIVELISTWITHSAVEDITERATORSUPPORTNODE_H

#include "LazyAndersenIntrusiveListNode.h"
#include "LazyAndersenSavedIterator.h"

namespace llvm {
namespace lazyandersen {
  template<typename NodeTy>
  struct IntrusiveListWithSavedIteratorSupportTraits;

  // Requests for the (get|set)(Next|Prev) methods of this class are ambiguous.
  // ilist_traits specializations must override the ilist_nextprev_traits
  // methods to disambiguate via the toNode() methods.
  template<typename NodeTy>
  class IntrusiveListWithSavedIteratorSupportNode :
      public IntrusiveListNode<NodeTy>, private ilist<SavedIterator<NodeTy> > {
    friend struct IntrusiveListWithSavedIteratorSupportTraits<NodeTy>;
    friend class SavedIterator<NodeTy>;

  public:
    IntrusiveListWithSavedIteratorSupportNode();

    IntrusiveListNode<NodeTy> *toNode() {
      return static_cast<IntrusiveListNode<NodeTy> *>(this);
    }

    const IntrusiveListNode<NodeTy> *toNode() const {
      return static_cast<const IntrusiveListNode<NodeTy> *>(this);
    }

    ilist<SavedIterator<NodeTy> > *getSavedIterators() {
      return static_cast<ilist<SavedIterator<NodeTy> > *>(this);
    }

    const ilist<SavedIterator<NodeTy> > *getSavedIterators() const {
      return static_cast<const ilist<SavedIterator<NodeTy> > *>(this);
    }
  };
}
}

#endif
