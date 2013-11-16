//===- LazyAndersenPointsToAlgorithm.h - points-to algorithm --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for the main points-to algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENPOINTSTOALGORITHM_H
#define LAZYANDERSENPOINTSTOALGORITHM_H

#include "LazyAndersenInstructionAnalysisAlgorithm.h"
#include "LazyAndersenLiteralAlgorithmId.h"
#include "LazyAndersenRelationType.h"

namespace llvm {
namespace lazyandersen {
  struct PointsToAlgorithm : public InstructionAnalysisAlgorithm {
    static const LiteralAlgorithmId ID;

    template<RelationType RT>
    struct RelationHandler;
  };
}
}

#endif
