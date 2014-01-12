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

  static ValueInfo *const Nil;

  ValueInfo(const Value *V);

  const Value *getValue() const {
    return V;
  }

  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;
  virtual bool isNodeHidden() const;

  void fillDebugInfo(DebugInfoFiller *DIF) const;
  void writeEquations(const DebugInfo &DI, raw_ostream &OS) const;

  template<typename AlgorithmTy, Phase CurrentPhase>
  AnalysisResult *getAlgorithmResult() {
    return GetAlgorithmResultHelper<
        AlgorithmTy::template IsEmptyIfMissing<CurrentPhase>::value>
            ::template getAlgorithmResult<AlgorithmTy>(this);
  }

  template<typename AlgorithmTy1, typename AlgorithmTy2>
  void addInstructionAnalysisWork(ValueInfo *that) {
    assert(!AlgorithmTy1::template IsEmptyIfMissing<
        INSTRUCTION_ANALYSIS_PHASE>::value);
    assert(!AlgorithmTy2::template IsEmptyIfMissing<
        INSTRUCTION_ANALYSIS_PHASE>::value);
    addInstructionAnalysisWorkInternal(&AlgorithmTy1::ID,
        &AlgorithmTy1::run, that, &AlgorithmTy2::ID,
        &AlgorithmTy2::run);
  }

private:
  virtual ~ValueInfo();

  template<bool IsEmptyIfMissing>
  struct GetAlgorithmResultHelper;

  AnalysisResult *getOrCreateAlgorithmResult(const AlgorithmId *Id,
      AlgorithmFn Fn);
  AnalysisResult *getAlgorithmResultOrNull(const AlgorithmId *Id) const;

  void addInstructionAnalysisWorkInternal(const AlgorithmId *Id1,
      AlgorithmFn Fn1, ValueInfo *that, const AlgorithmId *Id2,
      AlgorithmFn Fn2);
};

template<>
struct ValueInfo::GetAlgorithmResultHelper<false> {
  template<typename AlgorithmTy>
  static AnalysisResult *getAlgorithmResult(ValueInfo *VI) {
    return VI->getOrCreateAlgorithmResult(&AlgorithmTy::ID,
        &AlgorithmTy::run);
  }
};

template<>
struct ValueInfo::GetAlgorithmResultHelper<true> {
  template<typename AlgorithmTy>
  static AnalysisResult *getAlgorithmResult(ValueInfo *VI) {
    return VI->getAlgorithmResultOrNull(&AlgorithmTy::ID);
  }
};

}
}

#endif
