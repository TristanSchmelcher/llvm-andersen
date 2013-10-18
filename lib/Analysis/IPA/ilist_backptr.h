//===- ilist_backptr.h - ilist_node back-pointer to containing ilist ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines extensions to ilist_node to maintain a back-pointer to the
// containing ilist.
//
//===----------------------------------------------------------------------===//

#ifndef ILIST_BACKPTR_H
#define ILIST_BACKPTR_H

#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"

#include <cassert>

namespace llvm {
namespace lazyandersen {
  template<typename NodeTy> struct ilist_backptr_node_traits;

  template<typename NodeTy>
  class ilist_backptr {
    friend struct ilist_backptr_node_traits<NodeTy>;

    iplist<NodeTy> *List;

  protected:
    ilist_backptr() : List(0) {}

    iplist<NodeTy> *getList() const { return List; }

  private:
    void added(iplist<NodeTy> *ToList) {
      assert(!List);
      List = ToList;
    }

    void removed(iplist<NodeTy> *FromList) {
      assert(List == FromList);
      List = 0;
    }

    void transferred(iplist<NodeTy> *ToList, iplist<NodeTy> *FromList) {
      assert(List == FromList);
      List = ToList;
    }
  };

  // NodeTy must inherit from ilist_backptr.
  template<typename NodeTy>
  struct ilist_backptr_node_traits : public ilist_node_traits<NodeTy> {
    void addNodeToList(NodeTy *Node) {
      Node->added(static_cast<iplist<NodeTy> *>(this));
    }

    void removeNodeFromList(NodeTy *Node) {
      Node->removed(static_cast<iplist<NodeTy> *>(this));
    }

    void transferNodesFromList(
        ilist_node_traits<NodeTy> &that,
        ilist_iterator<NodeTy> first,
        ilist_iterator<NodeTy> last);
  };

  template<typename NodeTy>
  void ilist_backptr_node_traits<NodeTy>::transferNodesFromList(
      ilist_node_traits<NodeTy> &that,
      ilist_iterator<NodeTy> first,
      ilist_iterator<NodeTy> last) {
    for (ilist_iterator<NodeTy> i = first; i != last; ++i) {
      i->transferred(static_cast<iplist<NodeTy> *>(this),
          static_cast<iplist<NodeTy> *>(&that));
    }
  }
}
}

#endif
