//===- LazyAndersenHalfRelation-inl.h - relations classes -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for the template type for one direction
// of a relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENHALFRELATION_INL_H
#define LAZYANDERSENHALFRELATION_INL_H

#include "LazyAndersenHalfRelation.h"

#include "LazyAndersenHalfRelationBase-inl.h"
#include "LazyAndersenHalfRelationBaseList-inl.h"
#include "LazyAndersenHalfRelationList-inl.h"
#include "LazyAndersenRelation-inl.h"
#include "LazyAndersenValueInfo-inl.h"

namespace llvm {
namespace lazyandersen {
  template<RelationDirection Direction>
  inline const Relation *HalfRelation<Direction>::getRelation() const {
    return static_cast<const Relation *>(this);
  }

  template<RelationDirection Direction>
  inline Relation *HalfRelation<Direction>::getRelation() {
    return static_cast<Relation *>(this);
  }

  template<RelationDirection Direction>
  ValueInfo *HalfRelation<Direction>::getValueInfo() const {
    assert(getList());
    return getList()->as<Direction>()->getValueInfo();
  }

  template<RelationDirection Direction>
  const typename HalfRelation<Direction>::OppositeHalfRelationTy *
  HalfRelation<Direction>::getOppositeDirection() const {
    return getRelation()->template getDirection<OppositeDirection>();
  }

  template<RelationDirection Direction>
  typename HalfRelation<Direction>::OppositeHalfRelationTy *
  HalfRelation<Direction>::getOppositeDirection() {
    return getRelation()->template getDirection<OppositeDirection>();
  }

  template<RelationDirection Direction>
  inline HalfRelation<Direction>::HalfRelation(ValueInfo *VI)
    : HalfRelationBase(VI->getRelations<Direction>()) {}

  template<RelationDirection Direction>
  HalfRelation<Direction>::~HalfRelation() {}
}
}

#endif
