//===- LazyAndersenAnalysisResult.h - algorithm classes -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULT_H
#define LAZYANDERSENANALYSISRESULT_H

#include "LazyAndersenAlgorithmResultCache.h"
#include "LazyAndersenAnalysisResultAlgorithmId.h"
#include "LazyAndersenAnalysisResultEntryList.h"
#include "llvm/ADT/OwningPtr.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult : public AnalysisResultEntryList,
      public AlgorithmResultCache<AnalysisResultAlgorithmId> {
  public:
    typedef OwningPtr<AnalysisResult> Ref;

    AnalysisResult();
    ~AnalysisResult();
  };
}
}

#endif
