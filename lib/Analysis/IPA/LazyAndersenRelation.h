//===- LazyAndersenRelation.h - relation classes --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a base type for relations--i.e., directed edges in the
// analysis graph.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATION_H
#define LAZYANDERSENRELATION_H

#include "LazyAndersenEdgeEndpointType.h"
#include "LazyAndersenHalfRelation.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  class Relation :
      private HalfRelation<SOURCE>,
      private HalfRelation<DESTINATION> {
  public:
    Relation(ValueInfo *Src, ValueInfo *Dst);
    virtual ~Relation();

    template<EdgeEndpointType Endpoint>
    static Relation *get(HalfRelation<Endpoint> *HR) {
      return static_cast<Relation *>(HR);
    }

    template<EdgeEndpointType Endpoint>
    static const Relation *get(const HalfRelation<Endpoint> *HR) {
      return static_cast<const Relation *>(HR);
    }

    template<EdgeEndpointType Endpoint>
    static typename HalfRelationEdgeEndpointTraits<Endpoint>
        ::OppositeEndpointTy *
    getOppositeEndpoint(HalfRelation<Endpoint> *HR) {
      return get(HR)->template getHalf<
          HalfRelationEdgeEndpointTraits<Endpoint>::OppositeEndpoint>();
    }

    template<EdgeEndpointType Endpoint>
    static const typename HalfRelationEdgeEndpointTraits<Endpoint>
        ::OppositeEndpointTy *
    getOppositeEndpoint(const HalfRelation<Endpoint> *HR) {
      return get(HR)->template getHalf<
          HalfRelationEdgeEndpointTraits<Endpoint>::OppositeEndpoint>();
    }

    template<EdgeEndpointType Endpoint>
    HalfRelation<Endpoint> *getHalf() {
      return static_cast<HalfRelation<Endpoint> *>(this);
    }

    template<EdgeEndpointType Endpoint>
    const HalfRelation<Endpoint> *getHalf() const {
      return static_cast<const HalfRelation<Endpoint> *>(this);
    }

    template<EdgeEndpointType Endpoint>
    ValueInfo *getValueInfo() const {
      return getHalf<Endpoint>()->getValueInfo();
    }

    virtual const char *getRelationName() const = 0;
    virtual AnalysisResult *analyzePointsToSet() const = 0;
    virtual AnalysisResult *analyzeOutgoingReversePointsToSet() const = 0;
    virtual AnalysisResult *analyzeIncomingReversePointsToSet() const = 0;
    virtual AnalysisResult *analyzeStoredValuesPointsToSet() const = 0;
    virtual AnalysisResult *analyzeLoadedValuesReversePointsToSet() const = 0;
    virtual AnalysisResult *analyzeArgumentsPointsToSet() const = 0;
    virtual AnalysisResult *analyzeArgumentsReversePointsToSet() const = 0;
    virtual AnalysisResult *analyzeReturnValuePointsToSet() const = 0;
    virtual AnalysisResult *analyzeReturnValueReversePointsToSet() const = 0;
  };
}
}

#endif
