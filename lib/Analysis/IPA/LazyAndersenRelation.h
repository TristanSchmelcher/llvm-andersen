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
    template<RelationDirection Direction> friend class HalfRelation;

  public:
    Relation(ValueInfo *Src, ValueInfo *Dst);
    virtual ~Relation();

    template<RelationDirection Direction>
    const HalfRelation<Direction> *getDirection() const;

    template<RelationDirection Direction>
    HalfRelation<Direction> *getDirection();

    template<RelationDirection Direction>
    ValueInfo *getValueInfo() const;

    virtual const char *getRelationName() const = 0;
    virtual AnalysisResult *analyzePointsToSet() const = 0;
    virtual AnalysisResult *analyzeOutgoingReversePointsToSet() const = 0;
    virtual AnalysisResult *analyzeIncomingReversePointsToSet() const = 0;
  };
}
}

#endif
