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

namespace llvm {
namespace lazyandersen {
  class Relation;
  class ValueInfo;

  template<RelationDirection Direction>
  class HalfRelation : public HalfRelationBase,
      private DirectionTraits<Direction> {
    friend class Relation;

  public:
    using DirectionTraits<Direction>::OppositeDirection;
    typedef HalfRelation<OppositeDirection> OppositeHalfRelationTy;

    const Relation *getRelation() const;
    Relation *getRelation();
    ValueInfo *getValueInfo() const;
    const OppositeHalfRelationTy *getOppositeDirection() const;
    OppositeHalfRelationTy *getOppositeDirection();

  private:
    explicit HalfRelation(ValueInfo *VI);
    ~HalfRelation();
  };
}
}

#endif
