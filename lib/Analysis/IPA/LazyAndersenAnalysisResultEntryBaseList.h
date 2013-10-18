//===- LazyAndersenAnalysisResultEntryBaseList.h - analysis classes -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a list of analysis result entries.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTENTRYBASELIST_H
#define LAZYANDERSENANALYSISRESULTENTRYBASELIST_H

#include "LazyAndersenAnalysisResultEntryBase.h"
#include "llvm/ADT/ilist.h"

namespace llvm {
namespace lazyandersen {
  // Currently we do not need any extra information in these lists.
  typedef iplist<AnalysisResultEntryBase> AnalysisResultEntryBaseList;
}
}

#endif
