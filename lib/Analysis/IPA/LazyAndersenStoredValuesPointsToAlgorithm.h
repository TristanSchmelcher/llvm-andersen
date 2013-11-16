//===- LazyAndersenStoredValuesPointsToAlgorithm.h ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the stored values points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENSTOREDVALUESPOINTSTOALGORITHM_H
#define LAZYANDERSENSTOREDVALUESPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenLiteralAlgorithmId.h"
#include "LazyAndersenRelationType.h"

namespace llvm {
namespace lazyandersen {
  struct StoredValuesPointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const LiteralAlgorithmId ID;

    template<RelationType RT>
    struct RelationHandler;
  };
}
}

#endif
