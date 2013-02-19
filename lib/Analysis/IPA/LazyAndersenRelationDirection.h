//===- LazyAndersenRelationDirection.h - direction constants --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines direction constants for the LazyAndersen algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONDIRECTION_H
#define LAZYANDERSENRELATIONDIRECTION_H

#include "llvm/Support/Casting.h"

namespace llvm {
namespace lazyandersen {
  enum RelationDirection {
    INCOMING,
    OUTGOING
  };

  class HasDirection {
  public:
    virtual RelationDirection getDirection() const = 0;
    virtual ~HasDirection() {}
  };

  template<RelationDirection Direction>
  struct DirectionTraitsBase;

  template<>
  struct DirectionTraitsBase<INCOMING> {
    static const RelationDirection OppositeDirection = OUTGOING;
  };

  template<>
  struct DirectionTraitsBase<OUTGOING> {
    static const RelationDirection OppositeDirection = INCOMING;
  };

  template<RelationDirection Direction, typename BaseType,
      template<RelationDirection> class TemplateType>
  class DirectionTraits : public DirectionTraitsBase<Direction> {
  public:
    using DirectionTraitsBase<Direction>::OppositeDirection;
    typedef TemplateType<OppositeDirection> OppositeDirectionTy;

    static TemplateType<Direction> *from(BaseType *Base) {
      return cast<TemplateType<Direction> >(Base);
    }

    static const TemplateType<Direction> *from(const BaseType *Base) {
      return cast<TemplateType<Direction> >(Base);
    }

  protected:
    static bool classof(const BaseType *Base) {
      return Base->getDirection() == Direction;
    }
  };
}
}

#endif
