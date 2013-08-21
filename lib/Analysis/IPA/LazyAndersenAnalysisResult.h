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
#include "LazyAndersenAnalysisResultEntryBaseList.h"
#include "LazyAndersenGraphNode.h"
#include "llvm/ADT/OwningPtr.h"

#include <cassert>

namespace llvm {
namespace lazyandersen {
  class AnalysisResult : public AnalysisResultEntryBaseList,
      public GraphNode<GraphNodeBase::ANALYSIS_RESULT> {
    AlgorithmResultCache<AnalysisResultAlgorithmId> ResultCache;
    bool Enumerating;

  public:
    using GraphNode<GraphNodeBase::ANALYSIS_RESULT>::classof;

    class ScopedSetEnumeratingFlag {
      AnalysisResult *AR;

    public:
      ScopedSetEnumeratingFlag(AnalysisResult *AR) : AR(AR) {
        assert(!AR->Enumerating);
        AR->Enumerating = true;
      }

      ~ScopedSetEnumeratingFlag() {
        assert(AR->Enumerating);
        AR->Enumerating = false;
      }
    };

    typedef OwningPtr<AnalysisResult> Ref;

    AnalysisResult();
    virtual ~AnalysisResult();

    virtual std::list<GraphEdge> getOutgoingEdges() const;

    bool isEnumerating() const { return Enumerating; }

    template<AnalysisResultAlgorithmId Id>
    AnalysisResult *getAlgorithmResult() {
      return ResultCache.getAlgorithmResult<Id>(this);
    }
  };
}
}

#endif
