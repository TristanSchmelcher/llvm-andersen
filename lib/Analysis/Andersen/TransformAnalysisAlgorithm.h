//===- TransformAnalysisAlgorithm.h ---------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares template types for algorithms that transform a ValueInfo
// to a set by traversing the edges in the ValueInfo graph.
//
//===----------------------------------------------------------------------===//

#ifndef TRANSFORMANALYSISALGORITHM_H
#define TRANSFORMANALYSISALGORITHM_H

#include "IsNotNecessarilyEmptyIfMissingProperty.h"
#include "Phase.h"
#include "TraversalAlgorithmId.h"
#include "TransformAnalysisResult.h"
#include "ValueInfo.h"

namespace llvm {
namespace andersen_internal {

class TransformAnalysisBase {
  template<typename AlgorithmTy>
  class Result : public TransformAnalysisResult {
  public:
    explicit Result(InstructionAnalysisResult *IAR)
      : TransformAnalysisResult(IAR) {}

  private:
    virtual AnalysisResult *analyzeElement(ValueInfo *VI) const {
      return VI->getAlgorithmResult<AlgorithmTy, ENUMERATION_PHASE>();
    }

    virtual const AlgorithmId *getAlgorithmId() const {
      return &AlgorithmTy::ID;
    }
  };

  template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
           Phase RunPhase>
  struct TransformAnalysisAlgorithm :
      public IsNotNecessarilyEmptyIfMissingProperty {
    typedef AnalysisResult ResultTy;

    static const TraversalAlgorithmId ID;

    static AnalysisResult *run(ValueInfo *VI) {
      return new Result<SecondHopAlgorithm>(
          VI->getAlgorithmResult<FirstHopAlgorithm, RunPhase>());
    }
  };

  template<typename FirstHopAlgorithm, typename SecondHopAlgorithm>
  friend struct TwoFunctionTransform;

  template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
           typename ThirdHopAlgorithm>
  friend struct ThreeFunctionTransform;
};

template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
         Phase RunPhase>
const TraversalAlgorithmId TransformAnalysisBase::TransformAnalysisAlgorithm<
    FirstHopAlgorithm,
    SecondHopAlgorithm,
    RunPhase>::ID(&FirstHopAlgorithm::ID,
                  &SecondHopAlgorithm::ID);

template<typename FirstHopAlgorithm, typename SecondHopAlgorithm>
struct TwoFunctionTransform : private TransformAnalysisBase {
  typedef TransformAnalysisAlgorithm<FirstHopAlgorithm,
                                     SecondHopAlgorithm,
                                     INSTRUCTION_ANALYSIS_PHASE> Algorithm;
};

template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
         typename ThirdHopAlgorithm>
struct ThreeFunctionTransform : private TransformAnalysisBase {
  typedef TransformAnalysisAlgorithm<
      FirstHopAlgorithm,
      TransformAnalysisAlgorithm<SecondHopAlgorithm,
                                 ThirdHopAlgorithm,
                                 ENUMERATION_PHASE>,
      INSTRUCTION_ANALYSIS_PHASE> Algorithm;
};

}
}

#endif
