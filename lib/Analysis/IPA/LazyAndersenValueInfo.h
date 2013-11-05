//===- LazyAndersenValueInfo.h - value information ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type holding all information for a given Value.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENVALUEINFO_H
#define LAZYANDERSENVALUEINFO_H

#include "LazyAndersenAlgorithmResultCache.h"
#include "LazyAndersenGraphNode.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

namespace llvm {
  class Value;
}

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;
  typedef AlgorithmResultCache<ValueInfo, AnalysisResult, const char *>
      AnalysisResultCacheTy;

  class ValueInfo :
      private RefCountedBase<ValueInfo>,
      public GraphNode,
      private AnalysisResultCacheTy {
    friend struct IntrusiveRefCntPtrInfo<ValueInfo>;
    friend class RefCountedBase<ValueInfo>;
    friend class AlgorithmResultCache<ValueInfo, AnalysisResult, const char *>;
    // The Value that maps to this object. (If this analysis applies to
    // multiple Values, this is the first one that was analyzed.)
    const Value *V;

  public:
    typedef IntrusiveRefCntPtr<ValueInfo> Ref;

    using AnalysisResultCacheTy::getAlgorithmResult;
    using AnalysisResultCacheTy::getOrCreateEagerAlgorithmResult;
    using AnalysisResultCacheTy::preCreateSpecialCaseResult;

    static ValueInfo *const Nil;

    ValueInfo(const Value *V);

    const Value *getValue() const {
      return V;
    }

    virtual GraphEdgeDeque getOutgoingEdges() const;
    virtual std::string getNodeLabel() const;
    virtual bool isNodeHidden() const;

  private:
    virtual ~ValueInfo();
  };
}
}

#endif
