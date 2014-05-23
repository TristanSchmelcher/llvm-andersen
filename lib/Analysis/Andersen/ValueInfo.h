//===- ValueInfo.h - value information ------------------------------------===//
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

#ifndef VALUEINFO_H
#define VALUEINFO_H

#include "GraphNode.h"
#include "Phase.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include <cassert>

namespace llvm {

class raw_ostream;
class Value;

}

namespace llvm {
namespace andersen_internal {

class AlgorithmId;
class AnalysisResult;
class DebugInfoFiller;

class ValueInfo : private RefCountedBase<ValueInfo>, public GraphNode {
  friend struct IntrusiveRefCntPtrInfo<ValueInfo>;
  friend class RefCountedBase<ValueInfo>;
  typedef AnalysisResult *(*AlgorithmFn)(ValueInfo *);
  typedef DenseMap<const AlgorithmId *, AnalysisResult *> ResultsMapTy;
  ResultsMapTy Results;
  // The Value that maps to this object, or null for the special ValueInfos
  // owned by Data. (If this analysis applies to multiple Values, this is the
  // first one that was analyzed.)
  const Value *V;

public:
  typedef IntrusiveRefCntPtr<ValueInfo> Ref;

  ValueInfo(const Value *V);

  const Value *getValue() const {
    return V;
  }

  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;
  virtual bool isNodeHidden() const;

  void fillDebugInfo(DebugInfoFiller *DIF) const;
  void writeEquations(const DebugInfo &DI, raw_ostream &OS) const;

  template<Phase CurrentPhase, typename AlgorithmTy>
  AnalysisResult *getAlgorithmResult(const AlgorithmTy &Algorithm) {
    return GetAlgorithmResultHelper<
        AlgorithmTy::template IsEmptyIfMissing<CurrentPhase>::value>
            ::getAlgorithmResult(this, Algorithm);
  }

  template<typename AlgorithmTy1, typename AlgorithmTy2>
  void addInstructionAnalysisWork(const AlgorithmTy1 &Algorithm1,
      ValueInfo *that, const AlgorithmTy2 &Algorithm2) {
    assert(!AlgorithmTy1::template IsEmptyIfMissing<
        INSTRUCTION_ANALYSIS_PHASE>::value);
    assert(!AlgorithmTy2::template IsEmptyIfMissing<
        INSTRUCTION_ANALYSIS_PHASE>::value);
    getOrCreateAlgorithmResult(Algorithm1)->appendSubset(
        that->getOrCreateAlgorithmResult(Algorithm2));
  }

private:
  virtual ~ValueInfo();

  template<bool IsEmptyIfMissing>
  struct GetAlgorithmResultHelper;

  template<typename AlgorithmTy>
  AnalysisResult *getOrCreateAlgorithmResult(const AlgorithmTy& Algorithm) {
    AnalysisResult **AR = getOrCreateAlgorithmResultInternal(&Algorithm.ID);
    if (!*AR) {
      *AR = Algorithm.run(this);
    }
    return *AR;
  }

  AnalysisResult **getOrCreateAlgorithmResultInternal(const AlgorithmId *Id);
  AnalysisResult *getAlgorithmResultOrNull(const AlgorithmId *Id) const;
};

template<>
struct ValueInfo::GetAlgorithmResultHelper<false> {
  template<typename AlgorithmTy>
  static AnalysisResult *getAlgorithmResult(ValueInfo *VI,
                                            const AlgorithmTy& Algorithm) {
    return VI->getOrCreateAlgorithmResult(Algorithm);
  }
};

template<>
struct ValueInfo::GetAlgorithmResultHelper<true> {
  template<typename AlgorithmTy>
  static AnalysisResult *getAlgorithmResult(ValueInfo *VI,
                                            const AlgorithmTy& Algorithm) {
    return VI->getAlgorithmResultOrNull(&Algorithm.ID);
  }
};

}
}

#endif
