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

#include "LazyAndersenEdgeEndpointType.h"
#include "LazyAndersenHalfRelationBaseList.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  template<EdgeEndpointType Endpoint>
  class HalfRelationList;

  template<EdgeEndpointType Endpoint>
  class HalfRelationListEdgeEndpointTraits : public EdgeEndpointTraits<Endpoint,
      HalfRelationBaseList, HalfRelationList> {};

  template<EdgeEndpointType Endpoint>
  class HalfRelationList : public HalfRelationBaseList,
      public HalfRelationListEdgeEndpointTraits<Endpoint> {
    friend class ValueInfo;

  public:
    using HalfRelationListEdgeEndpointTraits<Endpoint>::classof;

    virtual EdgeEndpointType getEdgeEndpointType() const;

  private:
    HalfRelationList();
  };
}
}

#endif
