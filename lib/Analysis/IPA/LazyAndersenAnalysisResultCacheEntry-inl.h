//===- LazyAndersenAnalysisResultCacheEntry-inl.h - analysis classes ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the inline methods for a template type for entries in
// analysis results that cache some value.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTCACHEENTRY_INL_H
#define LAZYANDERSENANALYSISRESULTCACHEENTRY_INL_H

#include "LazyAndersenAnalysisResultCacheEntry.h"

namespace llvm {
namespace lazyandersen {
  template<typename CachedTy, AnalysisResultEntry::EntryType EntryTypeVal>
  AnalysisResultCacheEntry<CachedTy, EntryTypeVal>::AnalysisResultCacheEntry(
      CachedTy *CachedValue)
    : CachedValue(CachedValue) {}

  template<typename CachedTy, AnalysisResultEntry::EntryType EntryTypeVal>
  AnalysisResultEntry::EntryType
  AnalysisResultCacheEntry<CachedTy, EntryTypeVal>::getEntryType() const {
    return EntryTypeVal;
  }

  template<typename CachedTy, AnalysisResultEntry::EntryType EntryTypeVal>
  inline CachedTy *
  AnalysisResultCacheEntry<CachedTy, EntryTypeVal>::getCachedValue() const {
    return CachedValue;
  }
}
}

#endif


