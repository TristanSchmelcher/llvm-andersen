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
#include "LazyAndersenHalfRelationList.h"
#include "LazyAndersenRelationDirection.h"
#include "LazyAndersenValueInfoAlgorithmId.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace llvm {
  class Value;
}

namespace llvm {
namespace lazyandersen {
  class ValueInfo : private RefCountedBase<ValueInfo>,
      private HalfRelationList<INCOMING>,
      private HalfRelationList<OUTGOING> {
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

    template<RelationDirection Direction>
    static ValueInfo *get(HalfRelationList<Direction> *List) {
      return static_cast<ValueInfo *>(List);
    }

    template<RelationDirection Direction>
    static const ValueInfo *get(const HalfRelationList<Direction> *List) {
      return static_cast<const ValueInfo *>(List);
    }

    template<RelationDirection Direction>
    HalfRelationList<Direction> *getRelations() {
      return static_cast<HalfRelationList<Direction> *>(this);
    }

    template<RelationDirection Direction>
    const HalfRelationList<Direction> *getRelations() const {
      return static_cast<const HalfRelationList<Direction> *>(this);
    }

    template<ValueInfoAlgorithmId Id>
    AnalysisResult *getAlgorithmResult() {
      return ResultCache.getAlgorithmResult<Id>(this);
    }

  private:
    ~ValueInfo();
  };
}
}

#endif
