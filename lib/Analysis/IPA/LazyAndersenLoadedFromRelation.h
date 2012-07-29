//===- LazyAndersenLoadedFromRelation.h - loaded-from relation ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a loaded-from relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENLOADEDFROMRELATION_H
#define LAZYANDERSENLOADEDFROMRELATION_H

#include "LazyAndersenRelation.h"

namespace llvm {
namespace lazyandersen {
  class LoadedFromRelation : public Relation {
  public:
    LoadedFromRelation(ValueInfo *LoadedValueInfo,
        ValueInfo *AddressValueInfo);
    virtual const char *getRelationName() const;
    virtual AnalysisResult *analyzePointsToSet() const;
  };

  inline LoadedFromRelation::LoadedFromRelation(
      ValueInfo *LoadedValueInfo, ValueInfo *AddressValueInfo)
    : Relation(LoadedValueInfo, AddressValueInfo) {}
}
}

#endif
