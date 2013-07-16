//===- LazyAndersenEnumerator.cpp - lazy set enumerator -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an enumerator for lazy sets.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenEnumerator.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenAnalysisResultCacheEntry.h"
#include "LazyAndersenAnalysisResultPendingWorkEntry.h"
#include "LazyAndersenSavedIterator-inl.h"
#include "LazyAndersenValueInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

void Enumerator::enumerate(AnalysisResult *AR, DenseSet<const Value *> *Out) {
  if (AR->isEnumerating()) {
    return;
  }
  AnalysisResult::ScopedSetEnumeratingFlag Enumerating(AR);
  for (ilist<AnalysisResultEntry>::iterator i(AR->begin()); i != AR->end(); ) {
    switch (i->getEntryType()) {
    case AnalysisResultEntry::VALUE_INFO_ENTRY:
      Out->insert(cast<AnalysisResultValueInfoEntry>(&*i)->getCachedValue()
          ->getValue());
      ++i;
      break;

    case AnalysisResultEntry::RECURSIVE_ENTRY:
      enumerate(cast<AnalysisResultRecursiveEntry>(&*i)->getCachedValue(), Out);
      ++i;
      break;

    case AnalysisResultEntry::PENDING_WORK_ENTRY: {
      SavedIterator<AnalysisResultEntry> SI(AR);
      SI.set(i);
      cast<AnalysisResultPendingWorkEntry>(&*i)->run();
      i = SI.get();
      break;
    }

    default:
      llvm_unreachable("Not a recognized AnalysisResultEntry");
      break;
    }
  }
}
