//===- LazyAndersenAnalysisResultEntry.cpp - analysis classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for entries in analysis results.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenAnalysisResultEntry.h"

#include "LazyAndersenSavedIterator-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisResultEntry::AnalysisResultEntry() {}

AnalysisResultEntry::~AnalysisResultEntry() {
  assert(!getList());
}
