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

#include "LazyAndersenRecursiveEnumerate.h"

#include <cassert>

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisResult::EnumerationResult IterativeAnalysisStep::emit(
    AnalysisResult *Owner,
    AnalysisResultEntryBaseList::iterator *j,
    int Depth,
    AnalysisResult *Next) {
  RecursiveEnumerate *RE = new RecursiveEnumerate(Next);
  assert(AnalysisResultEntryBaseList::iterator(this) == *j);
  *j = Owner->insert(AnalysisResultEntryBaseList::iterator(this), RE);
  return RE->enumerate(Owner, j, Depth);
}
