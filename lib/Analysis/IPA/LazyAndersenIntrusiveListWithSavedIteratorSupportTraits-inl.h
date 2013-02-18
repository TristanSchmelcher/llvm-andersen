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
    ilist<SavedIterator<NodeTy> > *FromIteratorList = &Node->SavedIterators;
    if (!FromIteratorList->empty()) {
      // Advance the saved iterators pointing to this node. (Next ptr is still
      // valid during removeNodeFromList().)
      ilist_iterator<NodeTy> i(Node);
      if (++i != static_cast<ilist<NodeTy> *>(this)->end()) {
        ilist<SavedIterator<NodeTy> > *ToIteratorList = &i->SavedIterators;
        ToIteratorList->splice(ToIteratorList->end(), *FromIteratorList);
      } else {
        // End of list; nothing else after it.
        for (ilist_iterator<SavedIterator<NodeTy> > Current =
                 FromIteratorList->begin(), End = FromIteratorList->end();
             Current != End; ) {
          // remove advances the iterator.
          FromIteratorList->remove(Current);
        }
      }
    }
  }
}
}

#endif
