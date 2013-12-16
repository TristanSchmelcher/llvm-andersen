//===- LazyAndersenInstructionAnalysisAlgorithm.h -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a base type for algorithms that operate exclusively by
// hooking into the instruction analyzer.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINSTRUCTIONANALYSISALGORITHM_H
#define LAZYANDERSENINSTRUCTIONANALYSISALGORITHM_H

#include "LazyAndersenIsEmptyIfMissingInEnumerationPhaseProperty.h"

namespace llvm {
namespace andersen_internal {
  class AnalysisResult;
  class ValueInfo;

  struct InstructionAnalysisAlgorithm :
      public IsEmptyIfMissingInEnumerationPhaseProperty {
    static AnalysisResult *run(ValueInfo *VI);
  };
}
}

#endif
