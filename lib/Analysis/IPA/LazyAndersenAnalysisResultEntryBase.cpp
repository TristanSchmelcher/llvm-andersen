//===- LazyAndersenAnalysisResultEntryBase.cpp - analysis classes ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a base type for entries in analysis results.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenAnalysisResultEntryBase.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultCacheEntry.h"
#include "LazyAndersenAnalysisResultPendingWorkEntry.h"
#include "LazyAndersenValueInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisResultEntryBase::AnalysisResultEntryBase() {}

AnalysisResultEntryBase::~AnalysisResultEntryBase() {}

const GraphNode *AnalysisResultEntryBase::getGraphNode() const {
  switch (getEntryType()) {
  case AnalysisResultEntryBase::VALUE_INFO_ENTRY:
    return cast<AnalysisResultValueInfoEntry>(this)->getCachedValue();
  case AnalysisResultEntryBase::PENDING_WORK_ENTRY:
    return cast<AnalysisResultPendingWorkEntry>(this);
  default:
    llvm_unreachable("Unrecognized AnalysisResultEntryBase type");
    return 0;
  }
}
