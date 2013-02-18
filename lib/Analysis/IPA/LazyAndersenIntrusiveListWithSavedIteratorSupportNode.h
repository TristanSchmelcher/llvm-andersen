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
      public IntrusiveListNode<NodeTy> {
    friend struct IntrusiveListWithSavedIteratorSupportTraits<NodeTy>;

    // In a perfect world we would just use private inheritance here rather than
    // an extra class, but that causes ambiguity due to the C++ scoping rules.
    class SavedIteratorList : public ilist<SavedIterator<NodeTy> > {
      IntrusiveListWithSavedIteratorSupportNode *Owner;

    public:
      SavedIteratorList(IntrusiveListWithSavedIteratorSupportNode *Owner);
      IntrusiveListWithSavedIteratorSupportNode *getOwner() const;
    };

    SavedIteratorList SavedIterators;

  public:
    IntrusiveListWithSavedIteratorSupportNode();
  };
}
}

#endif
