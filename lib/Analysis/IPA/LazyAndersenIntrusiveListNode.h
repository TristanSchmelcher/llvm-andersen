//===- LazyAndersenIntrusiveListNode.h - ilist utilities ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an intrusive list node type for the LazyAndersen algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINTRUSIVELISTNODE_H
#define LAZYANDERSENINTRUSIVELISTNODE_H

#include "llvm/ADT/ilist_node.h"

#include <cassert>

namespace llvm {
  template<typename NodeTy> struct ilist;
}

namespace llvm {
namespace lazyandersen {
  template<typename NodeTy> struct IntrusiveListTraits;

  template<typename NodeTy>
  class IntrusiveListNode : public ilist_node<NodeTy> {
    friend struct IntrusiveListTraits<NodeTy>;

    ilist<NodeTy> *List;

  protected:
    IntrusiveListNode();
    ~IntrusiveListNode();
    ilist<NodeTy> *getList() const;

  private:
    void added(ilist<NodeTy> *ToList);
    void removed(ilist<NodeTy> *FromList);
    void transferred(ilist<NodeTy> *ToList, ilist<NodeTy> *FromList);
  };

  // class IntrusiveListNode
  template<typename NodeTy>
  IntrusiveListNode<NodeTy>::IntrusiveListNode() : List(0) {}

  template<typename NodeTy>
  IntrusiveListNode<NodeTy>::~IntrusiveListNode() {}

  template<typename NodeTy>
  inline ilist<NodeTy> *IntrusiveListNode<NodeTy>::getList() const {
    return List;
  }

  template<typename NodeTy>
  inline void IntrusiveListNode<NodeTy>::added(ilist<NodeTy> *ToList) {
    assert(!List);
    List = ToList;
  }

  template<typename NodeTy>
  inline void IntrusiveListNode<NodeTy>::removed(
      ilist<NodeTy> *FromList) {
    assert(List == FromList);
    List = 0;
  }

  template<typename NodeTy>
  inline void IntrusiveListNode<NodeTy>::transferred(
      ilist<NodeTy> *ToList, ilist<NodeTy> *FromList) {
    assert(List == FromList);
    List = ToList;
  }
}
}

#endif
