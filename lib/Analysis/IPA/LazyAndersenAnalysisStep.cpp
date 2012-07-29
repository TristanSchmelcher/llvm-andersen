//===- LazyAndersenAnalysisStep.cpp - analysis classes --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the class for one step of a lazy analysis algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenAnalysisStep.h"

#include "LazyAndersenAnalysisStepList.h"

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisStep::AnalysisStep() {}

AnalysisStep::~AnalysisStep() {
  assert(!getList());
}

AnalysisStep *AnalysisStep::getNextStep() {
  assert(getList());
  AnalysisStepList::iterator i(this);
  ++i;
  if (i == getList()->end()) {
    return 0;
  } else {
    return &*i;
  }
}
