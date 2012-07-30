//===- LazyAndersenHalfRelationList-inl.h - relations classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for the template type for a list of
// relations for a particular direction.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENHALFRELATIONLIST_INL_H
#define LAZYANDERSENHALFRELATIONLIST_INL_H

#include "LazyAndersenHalfRelationList.h"

#include "LazyAndersenHalfRelationBaseList-inl.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  template<RelationDirection Direction>
  inline const ValueInfo *HalfRelationList<Direction>::getValueInfo()
      const {
    return static_cast<const ValueInfo *>(this);
  }

  template<RelationDirection Direction>
  inline ValueInfo *HalfRelationList<Direction>::getValueInfo() {
    return static_cast<ValueInfo *>(this);
  }

  template<RelationDirection Direction>
  HalfRelationList<Direction>::HalfRelationList() {}
}
}

#endif
