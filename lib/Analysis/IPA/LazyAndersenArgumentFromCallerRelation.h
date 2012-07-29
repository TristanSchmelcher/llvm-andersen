//===- LazyAndersenArgumentFromCallerRelation.h - argument-from relation --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a argument-from-caller relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENARGUMENTFROMCALLERRELATION_H
#define LAZYANDERSENARGUMENTFROMCALLERRELATION_H

#include "LazyAndersenRelation.h"

namespace llvm {
namespace lazyandersen {
  class ArgumentFromCallerRelation : public Relation {
  public:
    ArgumentFromCallerRelation(ValueInfo *ArgumentValueInfo,
        ValueInfo *FunctionValueInfo);
    virtual const char *getRelationName() const;
    virtual AnalysisResult *analyzePointsToSet() const;
  };

  inline ArgumentFromCallerRelation::ArgumentFromCallerRelation(
      ValueInfo *ArgumentValueInfo,
      ValueInfo *FunctionValueInfo)
    : Relation(ArgumentValueInfo, FunctionValueInfo) {}
}
}

#endif
