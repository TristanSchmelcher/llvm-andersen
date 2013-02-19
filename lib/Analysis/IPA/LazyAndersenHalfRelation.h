//===- LazyAndersenHalfRelation.h - relations classes ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the template type for one direction of a relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENHALFRELATION_H
#define LAZYANDERSENHALFRELATION_H

#include "LazyAndersenHalfRelationBase.h"
#include "LazyAndersenRelationDirection.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  class Relation;
  class ValueInfo;

  template<RelationDirection Direction>
  class HalfRelation;

  template<RelationDirection Direction>
  class HalfRelationDirectionTraits : public DirectionTraits<Direction,
      HalfRelationBase, HalfRelation> {};

  template<RelationDirection Direction>
  class HalfRelation : public HalfRelationBase,
      public HalfRelationDirectionTraits<Direction> {
    friend class Relation;

  public:
    using HalfRelationDirectionTraits<Direction>::classof;

    virtual RelationDirection getDirection() const;

    ValueInfo *getValueInfo() const;

  private:
    explicit HalfRelation(ValueInfo *VI)
      : HalfRelationBase(VI->getRelations<Direction>()) {}
    ~HalfRelation();
  };
}
}

#endif
