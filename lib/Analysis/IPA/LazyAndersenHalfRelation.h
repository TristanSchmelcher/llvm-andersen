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

#include "LazyAndersenEdgeEndpointType.h"
#include "LazyAndersenHalfRelationBase.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  class Relation;
  class ValueInfo;

  template<EdgeEndpointType Endpoint>
  class HalfRelation;

  template<EdgeEndpointType Endpoint>
  class HalfRelationEdgeEndpointTraits : public EdgeEndpointTraits<Endpoint,
      HalfRelationBase, HalfRelation> {};

  template<EdgeEndpointType Endpoint>
  class HalfRelation : public HalfRelationBase,
      public HalfRelationEdgeEndpointTraits<Endpoint> {
    friend class Relation;

  public:
    using HalfRelationEdgeEndpointTraits<Endpoint>::classof;

    virtual EdgeEndpointType getEdgeEndpointType() const;

    ValueInfo *getValueInfo() const;

  private:
    explicit HalfRelation(ValueInfo *VI)
      : HalfRelationBase(VI->getRelations<Endpoint>()) {}
    ~HalfRelation();
  };
}
}

#endif
