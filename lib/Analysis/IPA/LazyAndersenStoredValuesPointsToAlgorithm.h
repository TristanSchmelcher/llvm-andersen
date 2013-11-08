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
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
namespace lazyandersen {
  struct StoredValuesPointsToAlgorithm :
      public InstructionAnalysisAlgorithm {
    static const char ID[];

    template<RelationType RT>
    struct RelationHandler {
      static void onRelation(ValueInfo *Src, ValueInfo *Dst) {}
    };
  };

  template<>
  struct StoredValuesPointsToAlgorithm::RelationHandler<STORED_TO> {
    static void onRelation(ValueInfo *Src, ValueInfo *Dst) {
      Dst->addInstructionAnalysisWork<StoredValuesPointsToAlgorithm,
          PointsToAlgorithm>(Src);
    }
  };
}
}

#endif
