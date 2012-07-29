//===- LazyAndersenHalfRelationBaseList-inl.h - relations classes ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for the base type for a list of
// relations for a particular direction.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENHALFRELATIONBASELIST_INL_H
#define LAZYANDERSENHALFRELATIONBASELIST_INL_H

#include "LazyAndersenHalfRelationBaseList.h"

#include "LazyAndersenHalfRelationList.h"

namespace llvm {
namespace lazyandersen {
  template<RelationDirection Direction>
  inline const HalfRelationList<Direction> *HalfRelationBaseList::as() const {
    return static_cast<const HalfRelationList<Direction> *>(this);
  }

  template<RelationDirection Direction>
  inline HalfRelationList<Direction> *HalfRelationBaseList::as() {
    return static_cast<HalfRelationList<Direction> *>(this);
  }
}
}

#endif
