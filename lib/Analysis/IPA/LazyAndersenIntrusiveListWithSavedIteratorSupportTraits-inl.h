//===- LazyAndersenIntrusiveListWithSavedIteratorsTraits-inl.h ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for
// LazyAndersenIntrusiveListWithSavedIteratorsTraits.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINTRUSIVELISTWITHSAVEDITERATORSUPPORTTRAITS_INL_H
#define LAZYANDERSENINTRUSIVELISTWITHSAVEDITERATORSUPPORTTRAITS_INL_H

#include "LazyAndersenIntrusiveListWithSavedIteratorSupportTraits.h"

#include "LazyAndersenSavedIterator.h"

namespace llvm {
namespace lazyandersen {
  template<typename NodeTy>
  void IntrusiveListWithSavedIteratorSupportTraits<NodeTy>::removeNodeFromList(
      NodeTy *Node) {
    IntrusiveListTraits<NodeTy>::removeNodeFromList(Node);
    if (!Node->getSavedIterators()->empty()) {
      // Advance the saved iterators pointing to this node. (Next ptr is still
      // valid during removeNodeFromList().)
      ilist_iterator<NodeTy> i(Node);
      if (++i != static_cast<ilist<NodeTy> *>(this)->end()) {
        i->getSavedIterators()->splice(i->getSavedIterators()->end(),
                                       *Node->getSavedIterators());
      } else {
        // End of list; nothing else after it.
        for (ilist_iterator<SavedIterator<NodeTy> >
                 Current = Node->getSavedIterators()->begin(),
                 End = Node->getSavedIterators()->end();
             Current != End; ) {
          // remove advances the iterator.
          Node->getSavedIterators()->remove(Current);
        }
      }
    }
  }
}
}

#endif
