//===- LazyAndersenLoadedValuesReversePointsToAlgorithm.h -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the loaded values reverse points-to
// algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENLOADEDVALUESREVERSEPOINTSTOALGORITHM_H
#define LAZYANDERSENLOADEDVALUESREVERSEPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenLiteralAlgorithmId.h"
#include "LazyAndersenRelationType.h"

namespace llvm {
namespace lazyandersen {
  struct LoadedValuesReversePointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const LiteralAlgorithmId ID;

    template<RelationType RT>
    struct RelationHandler;
  };
}
}

#endif
