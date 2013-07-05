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
  SavedIterator<NodeTy>::SavedIterator(ilist<NodeTy> *List,
                                       const ilist_iterator<NodeTy> &i)
    : List(List) {
    save(i);
  }

  template<typename NodeTy>
  SavedIterator<NodeTy>::~SavedIterator() {
    // restore() removes us from the saved iterators list that we are in.
    restore();
  }

  template<typename NodeTy>
  ilist_iterator<NodeTy> SavedIterator<NodeTy>::restore() {
    if (IntrusiveListNode<SavedIterator<NodeTy> >::getList()) {
      ilist_iterator<NodeTy> Ret = ilist_iterator<NodeTy>(static_cast<NodeTy *>(
          IntrusiveListNode<SavedIterator<NodeTy> >::getList()));
      IntrusiveListNode<SavedIterator<NodeTy> >::getList()->remove(
          typename ilist<SavedIterator<NodeTy> >::iterator(this));
      assert(!IntrusiveListNode<SavedIterator<NodeTy> >::getList());
      return Ret;
    } else {
      return List->end();
    }
  }

  template<typename NodeTy>
  void SavedIterator<NodeTy>::save(const ilist_iterator<NodeTy> &i) {
    assert(!IntrusiveListNode<SavedIterator<NodeTy> >::getList());
    if (i != List->end()) {
      i->getSavedIterators()->push_back(this);
      assert(IntrusiveListNode<SavedIterator<NodeTy> >::getList());
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
