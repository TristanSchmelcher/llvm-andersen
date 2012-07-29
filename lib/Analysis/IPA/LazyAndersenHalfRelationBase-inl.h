//===- LazyAndersenHalfRelationBase-inl.h - relations classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for HalfRelationBase.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENHALFRELATIONBASE_INL_H
#define LAZYANDERSENHALFRELATIONBASE_INL_H

#include "LazyAndersenHalfRelationBase.h"

#include "LazyAndersenHalfRelation.h"
#include "LazyAndersenHalfRelationBaseList.h"

namespace llvm {
namespace lazyandersen {
  template<RelationDirection Direction>
  inline const HalfRelation<Direction> *HalfRelationBase::as() const {
    return static_cast<const HalfRelation<Direction> *>(this);
  }

  template<RelationDirection Direction>
  inline HalfRelation<Direction> *HalfRelationBase::as() {
    return static_cast<HalfRelation<Direction> *>(this);
  }

  inline HalfRelationBaseList *HalfRelationBase::getList() const {
    return static_cast<HalfRelationBaseList *>(
        IntrusiveListNode::getList());
  }
}
}

#endif
