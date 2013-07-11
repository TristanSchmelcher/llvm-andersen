//===- LazyAndersenArgumentToCalleeRelation.h - argument-to relation ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a argument-to-callee relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENARGUMENTTOCALLEERELATION_H
#define LAZYANDERSENARGUMENTTOCALLEERELATION_H

#include "LazyAndersenRelation.h"

namespace llvm {
namespace lazyandersen {
  class ArgumentToCalleeRelation : public Relation {
  public:
    ArgumentToCalleeRelation(ValueInfo *ArgumentValueInfo,
        ValueInfo *CalledValueInfo);
    virtual const char *getRelationName() const;
    virtual AnalysisResult *analyzePointsToSet() const;
    virtual AnalysisResult *analyzeOutgoingReversePointsToSet() const;
    virtual AnalysisResult *analyzeIncomingReversePointsToSet() const;
    virtual AnalysisResult *analyzeStoredValuesPointsToSet() const;
  };

  inline ArgumentToCalleeRelation::ArgumentToCalleeRelation(
      ValueInfo *ArgumentValueInfo, ValueInfo *CalledValueInfo)
    : Relation(ArgumentValueInfo, CalledValueInfo) {}
}
}

#endif
