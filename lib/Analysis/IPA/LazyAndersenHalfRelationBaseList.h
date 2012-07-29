//===- LazyAndersenHalfRelationBaseList.h - relations classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the base type for a list of relations for a particular
// direction.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENHALFRELATIONBASELIST_H
#define LAZYANDERSENHALFRELATIONBASELIST_H

#include "LazyAndersenHalfRelationBase.h"
#include "LazyAndersenRelationDirection.h"

namespace llvm {
namespace lazyandersen {
  template<RelationDirection Direction> class HalfRelationList;

  class HalfRelationBaseList : public ilist<HalfRelationBase> {
    template<RelationDirection Direction> friend class HalfRelationList;

  public:
    template<RelationDirection Direction>
    const HalfRelationList<Direction> *as() const;

    template<RelationDirection Direction>
    HalfRelationList<Direction> *as();

  private:
    HalfRelationBaseList();
    ~HalfRelationBaseList();
  };
}
}

#endif
