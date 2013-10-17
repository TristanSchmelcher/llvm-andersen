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
  template<typename CachedTy, AnalysisResultEntryBase::EntryType EntryTypeVal>
  class AnalysisResultCacheEntry : public AnalysisResultEntry<EntryTypeVal> {
    // We assume here that--once created--each value will persist for the
    // lifetime of the LazyAndersenData. If not, we'll need weak pointers.
    CachedTy *CachedValue;

  public:
    using AnalysisResultEntry<EntryTypeVal>::classof;

    explicit AnalysisResultCacheEntry(CachedTy *CachedValue);

    CachedTy *getCachedValue() const {
      return CachedValue;
    }
  };

  class AnalysisResult;
  class ValueInfo;

  typedef AnalysisResultCacheEntry<ValueInfo,
      AnalysisResultEntryBase::VALUE_INFO_ENTRY> AnalysisResultValueInfoEntry;
}
}

#endif
