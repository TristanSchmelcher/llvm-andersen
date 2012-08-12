//===- LazyAndersenValueInfo-inl.h - value information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for the type holding all information for
// a given Value.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENVALUEINFO_INL_H
#define LAZYANDERSENVALUEINFO_INL_H

#include "LazyAndersenValueInfo.h"

#include "LazyAndersenAlgorithmResultCache-inl.h"
#include "LazyAndersenAnalysisResult.h"

namespace llvm {
namespace lazyandersen {
  inline const Value *ValueInfo::getValue() const {
    return V;
  }

  inline ValueInfo::Map *ValueInfo::getMap() const {
    return ContainingMap;
  }

  template<RelationDirection Direction>
  inline const HalfRelationList<Direction> *ValueInfo::getRelations()
      const {
    return static_cast<const HalfRelationList<Direction> *>(this);
  }

  template<RelationDirection Direction>
  inline HalfRelationList<Direction> *ValueInfo::getRelations() {
    return static_cast<HalfRelationList<Direction> *>(this);
  }
}
}

#endif
