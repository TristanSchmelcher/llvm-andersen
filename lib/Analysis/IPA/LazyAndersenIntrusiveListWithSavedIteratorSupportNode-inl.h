//===- LazyAndersenIntrusiveListWithSavedIteratorSupportNode-inl.h --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for
// IntrusiveListWithSavedIteratorSupportNode.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINTRUSIVELISTWITHSAVEDITERATORSUPPORTNODE_INL_H
#define LAZYANDERSENINTRUSIVELISTWITHSAVEDITERATORSUPPORTNODE_INL_H

#include "LazyAndersenIntrusiveListWithSavedIteratorSupportNode.h"

namespace llvm {
namespace lazyandersen {
  template<typename NodeTy>
  IntrusiveListWithSavedIteratorSupportNode<NodeTy>
      ::IntrusiveListWithSavedIteratorSupportNode() {}
}
}

#endif
