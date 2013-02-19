//===- LazyAndersenHalfRelationList.cpp - relations classes ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the template type for a list of relations for a particular
// direction.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenHalfRelationList.h"

using namespace llvm;
using namespace lazyandersen;

template<RelationDirection Direction>
RelationDirection HalfRelationList<Direction>::getDirection() const {
  return Direction;
}

template<RelationDirection Direction>
HalfRelationList<Direction>::HalfRelationList() {}

template class HalfRelationList<INCOMING>;
template class HalfRelationList<OUTGOING>;
