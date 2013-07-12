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
    virtual AnalysisResult *analyzeOutgoingReversePointsToSet() const;
    virtual AnalysisResult *analyzeIncomingReversePointsToSet() const;
    virtual AnalysisResult *analyzeStoredValuesPointsToSet() const;
    virtual AnalysisResult *analyzeLoadedValuesReversePointsToSet() const;
    virtual AnalysisResult *analyzeArgumentsPointsToSet() const;
    virtual AnalysisResult *analyzeArgumentsReversePointsToSet() const;
    virtual AnalysisResult *analyzeReturnValuePointsToSet() const;
  };

  inline LoadedFromRelation::LoadedFromRelation(
      ValueInfo *LoadedValueInfo, ValueInfo *AddressValueInfo)
    : Relation(LoadedValueInfo, AddressValueInfo) {}
}
}

#endif
