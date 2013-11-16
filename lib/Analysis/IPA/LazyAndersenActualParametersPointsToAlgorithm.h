//===- LazyAndersenActualParametersPointsToAlgorithm.h --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the actual parameters points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENACTUALPARAMETERSPOINTSTOALGORITHM_H
#define LAZYANDERSENACTUALPARAMETERSPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenLiteralAlgorithmId.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct ActualParametersPointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const LiteralAlgorithmId ID;

    template<RelationType RT>
    struct RelationHandler;
  };
}
}

#endif
