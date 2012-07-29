//===- LazyAndersenStoredToRelation.h - stored-to relation ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a stored-to relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENSTOREDTORELATION_H
#define LAZYANDERSENSTOREDTORELATION_H

#include "LazyAndersenRelation.h"

namespace llvm {
namespace lazyandersen {
  class StoredToRelation : public Relation {
  public:
    StoredToRelation(ValueInfo *StoredValueInfo,
        ValueInfo *AddressValueInfo);
    virtual const char *getRelationName() const;
    virtual AnalysisResult *analyzePointsToSet() const;
  };

  inline StoredToRelation::StoredToRelation(ValueInfo *StoredValueInfo,
      ValueInfo *AddressValueInfo)
    : Relation(StoredValueInfo, AddressValueInfo) {}
}
}

#endif