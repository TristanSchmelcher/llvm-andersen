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

#include "LazyAndersenAnalysisResultEntryList.h"

#include <cassert>

using namespace llvm;
using namespace llvm::lazyandersen;

void IterativeAnalysisStep::emit(AnalysisResultEntry *Entry) {
  assert(getList());
  getList()->insert(AnalysisResultEntryList::iterator(this), Entry);
}

void IterativeAnalysisStep::done() {
  assert(getList());
  // erase deletes this object.
  getList()->erase(this);
}
