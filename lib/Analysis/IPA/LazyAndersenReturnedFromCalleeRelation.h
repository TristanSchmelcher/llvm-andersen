//===- LazyAndersenReturnedFromCalleeRelation.h - returned-from relation --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a returned-from-callee relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRETURNEDFROMCALLEERELATION_H
#define LAZYANDERSENRETURNEDFROMCALLEERELATION_H

#include "LazyAndersenRelation.h"

namespace llvm {
namespace lazyandersen {
  class ReturnedFromCalleeRelation : public Relation {
  public:
    ReturnedFromCalleeRelation(ValueInfo *ReturnedValueInfo,
        ValueInfo *CalledValueInfo);
    virtual const char *getRelationName() const;
    virtual AnalysisResult *analyzePointsToSet() const;
    virtual AnalysisResult *analyzeOutgoingReversePointsToSet() const;
    virtual AnalysisResult *analyzeIncomingReversePointsToSet() const;
    virtual AnalysisResult *analyzeStoredValuesPointsToSet() const;
    virtual AnalysisResult *analyzeLoadedValuesReversePointsToSet() const;
    virtual AnalysisResult *analyzeArgumentsPointsToSet() const;
  };

  inline ReturnedFromCalleeRelation::ReturnedFromCalleeRelation(
      ValueInfo *ReturnedValueInfo, ValueInfo *CalledValueInfo)
    : Relation(ReturnedValueInfo, CalledValueInfo) {}
}
}

#endif
