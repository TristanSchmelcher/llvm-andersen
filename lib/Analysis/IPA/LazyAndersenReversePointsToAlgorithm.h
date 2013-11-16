//===- LazyAndersenReversePointsToAlgorithm.h ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenIsNotNecessarilyEmptyIfMissingProperty.h"
#include "LazyAndersenLiteralAlgorithmId.h"
#include "LazyAndersenRelationType.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  // Reverse points-to is like an InstructionAnalysisAlgorithm, except it is
  // irregular in that all ValueInfo's are implicitly in their own reverse
  // points-to set.
  struct ReversePointsToAlgorithm :
      public IsNotNecessarilyEmptyIfMissingProperty {
    static const LiteralAlgorithmId ID;

    static AnalysisResult *run(ValueInfo *VI);

    template<RelationType RT>
    struct RelationHandler;
  };
}
}

#endif
