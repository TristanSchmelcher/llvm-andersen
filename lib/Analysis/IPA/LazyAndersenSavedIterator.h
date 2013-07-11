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
  class SavedIterator : private IntrusiveListNode<SavedIterator<NodeTy> > {
    friend struct ilist_ghostly_sentinel_traits<SavedIterator>;
    friend struct ilist_nextprev_traits<SavedIterator>;
    friend struct ilist_node_traits<SavedIterator>;
    friend struct IntrusiveListTraits<SavedIterator>;
    // This is different from the superclass's getList(). The superclass's
    // getList() is the saved iterator list in the node that this saved iterator
    // points to, whereas this is for the list containing that node.
    ilist<NodeTy> *List;

  public:
    SavedIterator(ilist<NodeTy> *List);
    ~SavedIterator();

    ilist<NodeTy> *getList() const {
      return List;
    }

    ilist_iterator<NodeTy> get();
    void set(ilist_iterator<NodeTy> i);

  private:
    void clear();
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
