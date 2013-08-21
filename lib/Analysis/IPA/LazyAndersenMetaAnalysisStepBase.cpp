//===- LazyAndersenMetaAnalysisStepBase.cpp - analysis classes ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a base type for meta analysis--i.e., analysis of other
// analysis results.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenMetaAnalysisStepBase.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultCacheEntry.h"
#include "LazyAndersenAnalysisResultEntryBaseList.h"
#include "LazyAndersenAnalysisResultPendingWorkEntry.h"
#include "LazyAndersenSavedIterator-inl.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

MetaAnalysisStepBase::MetaAnalysisStepBase(
    AnalysisResult *AR) : SI(AR) {
  SI.set(AR->begin());
}

MetaAnalysisStepBase::~MetaAnalysisStepBase() {}

void MetaAnalysisStepBase::run() {
  for (AnalysisResultEntryBaseList::iterator i(SI.get());
       i != SI.getList()->end(); ) {
    switch (i->getEntryType()) {
    case AnalysisResultEntryBase::VALUE_INFO_ENTRY:
      emit(new AnalysisResultRecursiveEntry(analyzeValueInfo(
          cast<AnalysisResultValueInfoEntry>(&*i)->getCachedValue())));
      SI.set(++i);
      return;

    case AnalysisResultEntryBase::RECURSIVE_ENTRY:
      emit(new AnalysisResultRecursiveEntry(analyzeRecursive(
          cast<AnalysisResultRecursiveEntry>(&*i)->getCachedValue())));
      SI.set(++i);
      return;

    case AnalysisResultEntryBase::PENDING_WORK_ENTRY:
      cast<AnalysisResultPendingWorkEntry>(&*i)->run();
      i = SI.get();
      break;

    default:
      llvm_unreachable("Not a recognized AnalysisResultEntry");
      break;
    }
  }
  // No entries left to analyze.
  done();
}

std::list<GraphEdge> MetaAnalysisStepBase::getOutgoingEdges() const {
  std::list<GraphEdge> Result;
  std::ostringstream OSS;
  AnalysisResultEntryBaseList::const_iterator i(SI.get());
  if (i != SI.getList()->end()) {
    OSS << &*i;
  } else {
    OSS << "end";
  }
  OSS << " of SRC";
  Result.push_back(GraphEdge(static_cast<AnalysisResult *>(SI.getList()),
                             OSS.str()));
  return Result;
}
