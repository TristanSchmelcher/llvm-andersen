//===- LazyAndersenSavedIterator-inl.h - ilist utilities ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for SavedIterator.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENSAVEDITERATOR_INL_H
#define LAZYANDERSENSAVEDITERATOR_INL_H

#include "LazyAndersenSavedIterator.h"

#include "LazyAndersenIntrusiveListWithSavedIteratorSupportNode-inl.h"

namespace llvm {
namespace lazyandersen {
  template<typename NodeTy>
  SavedIterator<NodeTy>::SavedIterator(ilist<NodeTy> *List) : List(List) {}

  template<typename NodeTy>
  SavedIterator<NodeTy>::~SavedIterator() {
    clear();
  }

  template<typename NodeTy>
  void SavedIterator<NodeTy>::clear() {
    if (IntrusiveListNode<SavedIterator<NodeTy> >::getList()) {
      IntrusiveListNode<SavedIterator<NodeTy> >::getList()->remove(
          ilist<SavedIterator<NodeTy> >::iterator(this));
      assert(!IntrusiveListNode<SavedIterator<NodeTy> >::getList());
    }
  }

  template<typename NodeTy>
  void SavedIterator<NodeTy>::set(const ilist_iterator<NodeTy> &i) {
    clear();
    if (i != List->end()) {
      i->SavedIterators.push_back(this);
      assert(IntrusiveListNode<SavedIterator<NodeTy> >::getList());
    }
  }

  template<typename NodeTy>
  ilist_iterator<NodeTy> SavedIterator<NodeTy>::get() {
    if (IntrusiveListNode<SavedIterator<NodeTy> >::getList()) {
      return ilist_iterator<NodeTy>(
          static_cast<typename NodeTy::SavedIteratorList *>(
              IntrusiveListNode<SavedIterator<NodeTy> >::getList())
                  ->getOwner());
    } else {
      return List->end();
    }
  }
}
}

namespace llvm {
  template<typename NodeTy>
  void ilist_traits<lazyandersen::SavedIterator<NodeTy> >::deleteNode(
      lazyandersen::SavedIterator<NodeTy> *Node) {
    // SavedIterator should never be deleted through the tracking list.
    assert(false);
  }
}

#endif
