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
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
namespace lazyandersen {
  template<typename NodeTy>
  SavedIterator<NodeTy>::SavedIterator(ilist<NodeTy> *List) : List(List) {}

  template<typename NodeTy>
  SavedIterator<NodeTy>::~SavedIterator() {
    clear();
  }

  template<typename NodeTy>
  ilist_iterator<NodeTy> SavedIterator<NodeTy>::get() {
    if (IntrusiveListNode<SavedIterator<NodeTy> >::getList()) {
      return ilist_iterator<NodeTy>(static_cast<NodeTy *>(
          IntrusiveListNode<SavedIterator<NodeTy> >::getList()));
    } else {
      return List->end();
    }
  }

  template<typename NodeTy>
  void SavedIterator<NodeTy>::set(ilist_iterator<NodeTy> i) {
    clear();
    if (i != List->end()) {
      i->getSavedIterators()->push_back(this);
      assert(IntrusiveListNode<SavedIterator<NodeTy> >::getList());
    }
  }

  template<typename NodeTy>
  void SavedIterator<NodeTy>::clear() {
    if (IntrusiveListNode<SavedIterator<NodeTy> >::getList()) {
      IntrusiveListNode<SavedIterator<NodeTy> >::getList()->remove(
          typename ilist<SavedIterator<NodeTy> >::iterator(this));
      assert(!IntrusiveListNode<SavedIterator<NodeTy> >::getList());
    }
  }
}
}

namespace llvm {
  template<typename NodeTy>
  void ilist_traits<lazyandersen::SavedIterator<NodeTy> >::deleteNode(
      lazyandersen::SavedIterator<NodeTy> *Node) {
    llvm_unreachable(
        "SavedIterator should never be deleted through the tracking list");
  }
}

#endif
