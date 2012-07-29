//===- LazyAndersenHalfRelationList.h - relations classes -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the template type for a list of relations for a particular
// direction.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENHALFRELATIONLIST_H
#define LAZYANDERSENHALFRELATIONLIST_H

#include "LazyAndersenHalfRelationBaseList.h"
#include "LazyAndersenRelationDirection.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  template<RelationDirection Direction>
  class HalfRelationList : public HalfRelationBaseList {
    friend class ValueInfo;

  public:
    const ValueInfo *getValueInfo() const;
    ValueInfo *getValueInfo();

  private:
    HalfRelationList();
  };
}
}

#endif
