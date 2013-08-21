//===- LazyAndersenAnalysisResultCacheEntry.cpp - analysis classes --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the template type for entries in analysis results that
// cache some value.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenAnalysisResultCacheEntry.h"

using namespace llvm;
using namespace lazyandersen;

template<typename CachedTy, AnalysisResultEntryBase::EntryType EntryTypeVal>
AnalysisResultCacheEntry<CachedTy, EntryTypeVal>::AnalysisResultCacheEntry(
    CachedTy *CachedValue)
  : CachedValue(CachedValue) {}

template class AnalysisResultCacheEntry<ValueInfo,
    AnalysisResultEntryBase::VALUE_INFO_ENTRY>;
template class AnalysisResultCacheEntry<AnalysisResult,
    AnalysisResultEntryBase::RECURSIVE_ENTRY>;
