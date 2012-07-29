//===- LazyAndersenIterativeAnalysisStep.cpp - analysis classes -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for iterative analysis.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenIterativeAnalysisStep.h"

#include "LazyAndersenAnalysisStepList.h"

#include <cassert>

using namespace llvm;
using namespace llvm::lazyandersen;

IterativeAnalysisStep::Result IterativeAnalysisStep::nextIteration(
    AnalysisStep *Step) {
  assert(getList());
  getList()->insert(AnalysisStepList::iterator(this), Step);
  return Result(0, Step);
}

IterativeAnalysisStep::Result IterativeAnalysisStep::done() {
  assert(getList());
  AnalysisStep *Next = getNextStep();
  // erase deletes this object.
  getList()->erase(this);
  return Result(0, Next);
}
