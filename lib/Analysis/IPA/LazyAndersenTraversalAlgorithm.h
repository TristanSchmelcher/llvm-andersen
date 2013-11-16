//===- LazyAndersenTraversalAlgorithm.h -----------------------------------===//
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

#ifndef LAZYANDERSENTRAVERSALALGORITHM_H
#define LAZYANDERSENTRAVERSALALGORITHM_H

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenPhase.h"
#include "LazyAndersenIsNotNecessarilyEmptyIfMissingProperty.h"
#include "LazyAndersenTransformStep.h"
#include "LazyAndersenTraversalAlgorithmId.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct TraversalBase {
  private:
    template<typename FirstHopAlgorithm, typename SecondHopAlgorithm,
             Phase RunPhase>
    struct TraversalAlgorithm :
        public IsNotNecessarilyEmptyIfMissingProperty {
      static const TraversalAlgorithmId ID;

      static AnalysisResult *run(ValueInfo *VI) {
        AnalysisResult *AR = new AnalysisResult();
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
