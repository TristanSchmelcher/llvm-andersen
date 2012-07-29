//===- LazyAndersenReturnedToCallerRelation.cpp - returned-to relation ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a returned-to-caller relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReturnedToCallerRelation.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *ReturnedToCallerRelation::getRelationName() const {
  return "returned to caller";
}

AnalysisResult *ReturnedToCallerRelation::analyzePointsToSet() const {
  // No effect on points-to set.
  return 0;
}
