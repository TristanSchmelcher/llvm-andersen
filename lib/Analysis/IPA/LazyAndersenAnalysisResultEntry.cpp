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

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultCacheEntry.h"
#include "LazyAndersenAnalysisResultPendingWorkEntry.h"
#include "LazyAndersenSavedIterator-inl.h"
#include "LazyAndersenValueInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisResultEntry::AnalysisResultEntry() {}

AnalysisResultEntry::~AnalysisResultEntry() {
  assert(!getList());
}

const GraphNodeBase *AnalysisResultEntry::getGraphNode() const {
  switch (getEntryType()) {
  case AnalysisResultEntry::VALUE_INFO_ENTRY:
    return cast<AnalysisResultValueInfoEntry>(this)->getCachedValue();
  case AnalysisResultEntry::RECURSIVE_ENTRY:
    return cast<AnalysisResultRecursiveEntry>(this)->getCachedValue();
  case AnalysisResultEntry::PENDING_WORK_ENTRY:
    return cast<AnalysisResultPendingWorkEntry>(this);
  default:
    llvm_unreachable("Unrecognized AnalysisResultEntry type");
    return 0;
  }
}
