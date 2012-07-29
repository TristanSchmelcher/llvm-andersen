//===- LazyAndersenArgumentToCalleeRelation.cpp - argument-to relation ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for a argument-to-callee relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenArgumentToCalleeRelation.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char *ArgumentToCalleeRelation::getRelationName() const {
  return "argument to callee";
}

AnalysisResult *ArgumentToCalleeRelation::analyzePointsToSet() const {
  // No effect on points-to set.
  return 0;
}
