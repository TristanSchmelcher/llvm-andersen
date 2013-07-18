//===- LazyAndersenEdgeEndpointType.h - graph edge constants --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines edge endpoint constants and traits for the LazyAndersen
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENEDGEENDPOINTTYPE_H
#define LAZYANDERSENEDGEENDPOINTTYPE_H

#include "llvm/Support/Casting.h"

namespace llvm {
namespace lazyandersen {
  enum EdgeEndpointType {
    SOURCE,
    DESTINATION
  };

  class HasEdgeEndpointType {
  public:
    virtual EdgeEndpointType getEdgeEndpointType() const = 0;
    virtual ~HasEdgeEndpointType() {}
  };

  template<EdgeEndpointType Endpoint>
  struct EdgeEndpointTraitsBase;

  template<>
  struct EdgeEndpointTraitsBase<SOURCE> {
    static const EdgeEndpointType OppositeEndpoint = DESTINATION;
  };

  template<>
  struct EdgeEndpointTraitsBase<DESTINATION> {
    static const EdgeEndpointType OppositeEndpoint = SOURCE;
  };

  template<EdgeEndpointType Endpoint, typename BaseType,
      template<EdgeEndpointType> class TemplateType>
  class EdgeEndpointTraits : public EdgeEndpointTraitsBase<Endpoint> {
  public:
    using EdgeEndpointTraitsBase<Endpoint>::OppositeEndpoint;
    typedef TemplateType<OppositeEndpoint> OppositeEndpointTy;

    static TemplateType<Endpoint> *from(BaseType *Base) {
      return cast<TemplateType<Endpoint> >(Base);
    }

    static const TemplateType<Endpoint> *from(const BaseType *Base) {
      return cast<TemplateType<Endpoint> >(Base);
    }

  protected:
    static bool classof(const BaseType *Base) {
      return Base->getEdgeEndpointType() == Endpoint;
    }
  };
}
}

#endif
