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

#include "LazyAndersenEdgeEndpointType.h"
#include "LazyAndersenHalfRelationBase.h"
#include "llvm/ADT/ilist.h"

namespace llvm {
namespace lazyandersen {
  template<EdgeEndpointType Endpoint> class HalfRelationList;

  class HalfRelationBaseList : public HasEdgeEndpointType,
      public ilist<HalfRelationBase> {
    template<EdgeEndpointType Endpoint> friend class HalfRelationList;

  public:
    static HalfRelationBaseList *get(ilist<HalfRelationBase> *List) {
      return static_cast<HalfRelationBaseList *>(List);
    }

    static const HalfRelationBaseList *get(
        const ilist<HalfRelationBase> *List) {
      return static_cast<const HalfRelationBaseList *>(List);
    }

  private:
    HalfRelationBaseList();
    ~HalfRelationBaseList();
  };
}
}

#endif
