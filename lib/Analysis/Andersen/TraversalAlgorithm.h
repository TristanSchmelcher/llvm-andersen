//===- TraversalAlgorithm.h -----------------------------------------------===//
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

#ifndef TRAVERSALALGORITHM_H
#define TRAVERSALALGORITHM_H

#include "AnalysisResult.h"
#include "IsNotNecessarilyEmptyIfMissingProperty.h"
#include "Phase.h"
#include "TransformStep.h"
#include "TraversalAlgorithmId.h"
#include "ValueInfo.h"

namespace llvm {
namespace andersen_internal {

struct TraversalBase {
private:
  template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
           Phase RunPhase>
  struct TraversalAlgorithm :
      public IsNotNecessarilyEmptyIfMissingProperty {
    static const TraversalAlgorithmId ID;

    static AnalysisResult *run(ValueInfo *VI) {
      AnalysisResult *AR = new AnalysisResult(AnalysisResultId(&ID, VI));
      AR->addWork(new TransformStep<SecondHopAlgorithm>(
          VI->getAlgorithmResult<FirstHopAlgorithm, RunPhase>()));
      return AR;
    }
  };

  template<typename FirstHopAlgorithm, typename SecondHopAlgorithm>
  friend struct TwoHopTraversal;

  template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
           typename ThirdHopAlgorithm>
  friend struct ThreeHopTraversal;
};

template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
         Phase RunPhase>
const TraversalAlgorithmId TraversalBase::TraversalAlgorithm<
    FirstHopAlgorithm,
    SecondHopAlgorithm,
    RunPhase>::ID(&FirstHopAlgorithm::ID,
                  &SecondHopAlgorithm::ID);

template<typename FirstHopAlgorithm, typename SecondHopAlgorithm>
struct TwoHopTraversal : private TraversalBase {
  typedef TraversalAlgorithm<FirstHopAlgorithm,
                             SecondHopAlgorithm,
                             INSTRUCTION_ANALYSIS_PHASE> Algorithm;
};

template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
         typename ThirdHopAlgorithm>
struct ThreeHopTraversal : private TraversalBase {
  typedef TraversalAlgorithm<
      FirstHopAlgorithm,
      TraversalAlgorithm<SecondHopAlgorithm,
                         ThirdHopAlgorithm,
                         ENUMERATION_PHASE>,
      INSTRUCTION_ANALYSIS_PHASE> Algorithm;
};

}
}

#endif
