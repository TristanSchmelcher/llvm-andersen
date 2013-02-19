//===- LazyAndersenHalfRelation.cpp - relations classes -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the template type for one direction of a relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenHalfRelation.h"

using namespace llvm;
using namespace lazyandersen;

template<RelationDirection Direction>
RelationDirection HalfRelation<Direction>::getDirection() const {
  return Direction;
}

template<RelationDirection Direction>
ValueInfo *HalfRelation<Direction>::getValueInfo() const {
  assert(getList());
  return ValueInfo::get(HalfRelationList<Direction>::from(
      HalfRelationBaseList::get(getList())));
}

template<RelationDirection Direction>
HalfRelation<Direction>::~HalfRelation() {}

template class HalfRelation<INCOMING>;
template class HalfRelation<OUTGOING>;
