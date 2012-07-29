//===- LazyAndersenReturnedToCallerRelation.h - returned-to relation ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a returned-to-caller relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRETURNEDTOCALLERRELATION_H
#define LAZYANDERSENRETURNEDTOCALLERRELATION_H

#include "LazyAndersenRelation.h"

namespace llvm {
namespace lazyandersen {
  class ReturnedToCallerRelation : public Relation {
  public:
    ReturnedToCallerRelation(ValueInfo *ReturnedValueInfo,
        ValueInfo *FunctionValueInfo);
    virtual const char *getRelationName() const;
    virtual AnalysisResult *analyzePointsToSet() const;
  };

  inline ReturnedToCallerRelation::ReturnedToCallerRelation(
      ValueInfo *ReturnedValueInfo,
      ValueInfo *FunctionValueInfo)
    : Relation(ReturnedValueInfo, FunctionValueInfo) {}
}
}

#endif