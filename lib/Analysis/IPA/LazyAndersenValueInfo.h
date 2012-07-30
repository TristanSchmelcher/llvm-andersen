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
      private HalfRelationList<OUTGOING>,
      public AlgorithmResultCache<ValueInfoAlgorithmId> {
    friend struct IntrusiveRefCntPtrInfo<ValueInfo>;
    friend class RefCountedBase<ValueInfo>;
    template<RelationDirection Direction> friend class HalfRelationList;
    // The Value that maps to this object. (If this analysis applies to
    // multiple Values, this is the first one that was analyzed.)
    const Value *V;

  public:
    typedef IntrusiveRefCntPtr<ValueInfo> Ref;
    // TODO: Should this be a ValueMap?
    typedef DenseMap<const Value *, Ref> Map;

  private:
    // The map that this analysis is in.
    Map *ContainingMap;

  public:
    static ValueInfo *const Nil;

    ValueInfo(const Value *V, Map *Map);
    const Value *getValue() const;
    Map *getMap() const;

    template<RelationDirection Direction>
    const HalfRelationList<Direction> *getRelations() const;

    template<RelationDirection Direction>
    HalfRelationList<Direction> *getRelations();

  private:
    ~ValueInfo();
  };
}
}

#endif
