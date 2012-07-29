//===- LazyAndersenRelation-inl.h - relation classes ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for a base type for relations--i.e.,
// directed edges in the analysis graph.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATION_INL_H
#define LAZYANDERSENRELATION_INL_H

#include "LazyAndersenRelation.h"

#include "LazyAndersenHalfRelation-inl.h"

namespace llvm {
namespace lazyandersen {
  template<RelationDirection Direction>
  inline const HalfRelation<Direction> *Relation::getDirection() const {
    return static_cast<const HalfRelation<Direction> *>(this);
  }

  template<RelationDirection Direction>
  inline HalfRelation<Direction> *Relation::getDirection() {
    return static_cast<HalfRelation<Direction> *>(this);
  }

  template<RelationDirection Direction>
  inline ValueInfo *Relation::getValueInfo() const {
    return getDirection<Direction>()->getValueInfo();
  }
}
}

#endif
