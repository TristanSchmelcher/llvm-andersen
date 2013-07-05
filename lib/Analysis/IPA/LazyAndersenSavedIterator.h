//===- LazyAndersenSavedIterator.h - ilist utilities ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the saved iterator type for use with an
// IntrusiveListWithSavedIteratorSupportNode.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENSAVEDITERATOR_H
#define LAZYANDERSENSAVEDITERATOR_H

#include "LazyAndersenIntrusiveListNode.h"
#include "LazyAndersenIntrusiveListTraits.h"

namespace llvm {
namespace lazyandersen {
  template<typename NodeTy>
  class ScopedLoadStoreIterator;

  template<typename NodeTy>
  class SavedIterator : private IntrusiveListNode<SavedIterator<NodeTy> > {
    friend struct ilist_nextprev_traits<SavedIterator>;
    friend struct ilist_node_traits<SavedIterator>;
    friend struct IntrusiveListTraits<SavedIterator>;
    friend class ScopedLoadStoreIterator<NodeTy>;
    // This is different from the superclass's getList(). The superclass's
    // getList() is the saved iterator list in the node that this saved iterator
    // points to, whereas this is for the list containing that node.
    ilist<NodeTy> *List;

  public:
    SavedIterator(ilist<NodeTy> *List, const ilist_iterator<NodeTy> &i);
    ~SavedIterator();

    ilist<NodeTy> *getList() const {
      return List;
    }

  private:
    ilist_iterator<NodeTy> restore();
    void save(const ilist_iterator<NodeTy> &i);
  };

  // Automatically restores from a saved iterator at construction time and saves
  // to that saved iterator at destruction time. The behaviour is undefined if
  // the iterator is changed to point to a different list.
  template<typename NodeTy>
  class ScopedLoadStoreIterator : public ilist_iterator<NodeTy> {
    SavedIterator<NodeTy> *Iterator;

  public:
    ScopedLoadStoreIterator(SavedIterator<NodeTy> *Iterator)
      : ilist_iterator<NodeTy>(Iterator->restore()), Iterator(Iterator) {}

    ~ScopedLoadStoreIterator() {
      Iterator->save(*this);
    }
  };
}
}

namespace llvm {
  template<typename NodeTy>
  struct ilist_traits<lazyandersen::SavedIterator<NodeTy> >
      : public lazyandersen::IntrusiveListTraits<
            lazyandersen::SavedIterator<NodeTy> > {
    static void deleteNode(lazyandersen::SavedIterator<NodeTy> *Node);
  };
}

#endif
