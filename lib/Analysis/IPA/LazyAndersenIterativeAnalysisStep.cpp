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

#include "LazyAndersenAnalysisResultEntryBaseList.h"

#include <cassert>

using namespace llvm;
using namespace llvm::lazyandersen;

void IterativeAnalysisStep::emit(AnalysisResultEntryBase *Entry) {
  assert(getList());
  getList()->insert(AnalysisResultEntryBaseList::iterator(this), Entry);
  // Move any saved iterators onto the emitted entry.
  Entry->getSavedIterators()->splice(Entry->getSavedIterators()->end(),
                                     *getSavedIterators());
}

void IterativeAnalysisStep::done() {
  assert(getList());
  // erase deletes this object.
  getList()->erase(this);
}
