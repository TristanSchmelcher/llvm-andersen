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

#include "LazyAndersenHalfRelation.h"
#include "LazyAndersenRelationDirection.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  class Relation :
      private HalfRelation<INCOMING>,
      private HalfRelation<OUTGOING> {
  public:
    Relation(ValueInfo *Src, ValueInfo *Dst);
    virtual ~Relation();

    template<RelationDirection Direction>
    static Relation *get(HalfRelation<Direction> *HR) {
      return static_cast<Relation *>(HR);
    }

    template<RelationDirection Direction>
    static const Relation *get(const HalfRelation<Direction> *HR) {
      return static_cast<const Relation *>(HR);
    }

    template<RelationDirection Direction>
    static typename HalfRelationDirectionTraits<Direction>
        ::OppositeDirectionTy *
    getOppositeDirection(HalfRelation<Direction> *HR) {
      return get(HR)->template getHalf<
          HalfRelationDirectionTraits<Direction>::OppositeDirection>();
    }

    template<RelationDirection Direction>
    static const typename HalfRelationDirectionTraits<Direction>
        ::OppositeDirectionTy *
    getOppositeDirection(const HalfRelation<Direction> *HR) {
      return get(HR)->template getHalf<
          HalfRelationDirectionTraits<Direction>::OppositeDirection>();
    }

    template<RelationDirection Direction>
    HalfRelation<Direction> *getHalf() {
      return static_cast<HalfRelation<Direction> *>(this);
    }

    template<RelationDirection Direction>
    const HalfRelation<Direction> *getHalf() const {
      return static_cast<const HalfRelation<Direction> *>(this);
    }

    template<RelationDirection Direction>
    ValueInfo *getValueInfo() const {
      return getHalf<Direction>()->getValueInfo();
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
  };
}
}

#endif
