//===- LazyAndersenAnalysisResultCacheEntry.h - algorithm classes ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a template type for entries in analysis results that cache
// some value.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTCACHEENTRY_H
#define LAZYANDERSENANALYSISRESULTCACHEENTRY_H

#include "LazyAndersenAnalysisResultEntry.h"

namespace llvm {
namespace lazyandersen {
  template<typename CachedTy, AnalysisResultEntry::EntryType EntryTypeVal>
  class AnalysisResultCacheEntry : public AnalysisResultEntry {
    // We assume here that--once created--each value will persist for the
    // lifetime of the LazyAndersenData. If not, we'll need weak pointers.
    CachedTy *CachedValue;

  public:
    explicit AnalysisResultCacheEntry(CachedTy *CachedValue);
    virtual EntryType getEntryType() const;
    CachedTy *getCachedValue() const;
  };

  class AnalysisResult;
  class ValueInfo;

  typedef AnalysisResultCacheEntry<ValueInfo,
      AnalysisResultEntry::VALUE_INFO_ENTRY> AnalysisResultValueInfoEntry;
  typedef AnalysisResultCacheEntry<AnalysisResult,
      AnalysisResultEntry::RECURSIVE_ENTRY> AnalysisResultRecursiveEntry;
}
}

#endif
