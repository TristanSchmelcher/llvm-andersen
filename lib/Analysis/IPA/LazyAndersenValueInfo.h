//===- LazyAndersenValueInfo.h - value information ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type holding all information for a given Value.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENVALUEINFO_H
#define LAZYANDERSENVALUEINFO_H

#include "LazyAndersenAlgorithmResultCache.h"
#include "LazyAndersenEdgeEndpointType.h"
#include "LazyAndersenGraphNode.h"
#include "LazyAndersenHalfRelationList.h"
#include "LazyAndersenValueInfoAlgorithmId.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace llvm {
  class Value;
}

namespace llvm {
namespace lazyandersen {
  class ValueInfo : private RefCountedBase<ValueInfo>,
      private HalfRelationList<SOURCE>,
      private HalfRelationList<DESTINATION>,
      public GraphNode {
    friend struct IntrusiveRefCntPtrInfo<ValueInfo>;
    friend class RefCountedBase<ValueInfo>;
    // The Value that maps to this object. (If this analysis applies to
    // multiple Values, this is the first one that was analyzed.)
    const Value *V;

  public:
    typedef IntrusiveRefCntPtr<ValueInfo> Ref;
    // TODO: Should this be a ValueMap?
    typedef DenseMap<const Value *, Ref> Map;

  private:
    AlgorithmResultCache<ValueInfoAlgorithmId> ResultCache;
    // The map that this analysis is in.
    Map *ContainingMap;

  public:
    static ValueInfo *const Nil;

    ValueInfo(const Value *V, Map *Map);

    const Value *getValue() const {
      return V;
    }

    Map *getMap() const {
      return ContainingMap;
    }

    template<EdgeEndpointType Endpoint>
    static ValueInfo *get(HalfRelationList<Endpoint> *List) {
      return static_cast<ValueInfo *>(List);
    }

    template<EdgeEndpointType Endpoint>
    static const ValueInfo *get(const HalfRelationList<Endpoint> *List) {
      return static_cast<const ValueInfo *>(List);
    }

    template<EdgeEndpointType Endpoint>
    HalfRelationList<Endpoint> *getRelations() {
      return static_cast<HalfRelationList<Endpoint> *>(this);
    }

    template<EdgeEndpointType Endpoint>
    const HalfRelationList<Endpoint> *getRelations() const {
      return static_cast<const HalfRelationList<Endpoint> *>(this);
    }

    template<ValueInfoAlgorithmId Id>
    AnalysisResult *getAlgorithmResult() {
      return ResultCache.getAlgorithmResult<Id>(this);
    }

    virtual GraphEdgeDeque getOutgoingEdges() const;
    virtual std::string getNodeLabel() const;
    virtual bool isNodeHidden() const;

    void setAlgorithmResultSpecialCase(ValueInfoAlgorithmId Id,
        AnalysisResult *AR);

  private:
    ~ValueInfo();
  };
}
}

#endif
