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

  template<typename NodeTy>
  class IntrusiveListWithSavedIteratorSupportNode :
      public IntrusiveListNode<NodeTy>, private ilist<SavedIterator<NodeTy> > {
    friend struct ilist_nextprev_traits<NodeTy>;
    friend struct IntrusiveListWithSavedIteratorSupportTraits<NodeTy>;
    friend class SavedIterator<NodeTy>;

  protected:
    // Disambiguate (get|set)(Prev|Next) node for ilist_nextprev_traits.
    using IntrusiveListNode<NodeTy>::getPrev;
    using IntrusiveListNode<NodeTy>::setPrev;
    using IntrusiveListNode<NodeTy>::getNext;
    using IntrusiveListNode<NodeTy>::setNext;

  public:
    IntrusiveListWithSavedIteratorSupportNode();

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
