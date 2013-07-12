//===- LazyAndersenDependsOnRelation.h - depends-on relation --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a depends-on relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENDEPENDSONRELATION_H
#define LAZYANDERSENDEPENDSONRELATION_H

#include "LazyAndersenRelation.h"

namespace llvm {
namespace lazyandersen {
  class DependsOnRelation : public Relation {
  public:
    DependsOnRelation(ValueInfo *DependentValueInfo,
        ValueInfo *IndependentValueInfo);
    virtual const char *getRelationName() const;
    virtual AnalysisResult *analyzePointsToSet() const;
    virtual AnalysisResult *analyzeOutgoingReversePointsToSet() const;
    virtual AnalysisResult *analyzeIncomingReversePointsToSet() const;
    virtual AnalysisResult *analyzeStoredValuesPointsToSet() const;
    virtual AnalysisResult *analyzeLoadedValuesReversePointsToSet() const;
  };

  inline DependsOnRelation::DependsOnRelation(
      ValueInfo *DependentValueInfo,
      ValueInfo *IndependentValueInfo)
    : Relation(DependentValueInfo, IndependentValueInfo) {}
}
}

#endif
