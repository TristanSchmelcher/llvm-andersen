//===- DropCallSitesTransformAlgorithm.h ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef DROPCALLSITESTRANSFORMALGORITHM_H
#define DROPCALLSITESTRANSFORMALGORITHM_H

#include "AnalysisResult.h"
#include "DropCallSitesAlgorithmId.h"
#include "DropCallSitesTransformWork.h"
#include "IsNotNecessarilyEmptyIfMissingProperty.h"
#include "Phase.h"
#include "ValueInfo.h"

namespace llvm {
namespace andersen_internal {

template<typename FirstHopAlgorithm>
struct DropCallSitesTransformAlgorithm :
    public IsNotNecessarilyEmptyIfMissingProperty {
  static const DropCallSitesAlgorithmId ID;

  static AnalysisResult *run(ValueInfo *VI) {
    AnalysisResult *AR = new AnalysisResult();
    AR->appendUniqueTransform(new DropCallSitesTransformWork(
        VI->getAlgorithmResult<INSTRUCTION_ANALYSIS_PHASE>(
            FirstHopAlgorithm())));
    return AR;
  }
};

template<typename FirstHopAlgorithm>
const DropCallSitesAlgorithmId
DropCallSitesTransformAlgorithm<FirstHopAlgorithm>::ID(&FirstHopAlgorithm::ID);

}
}

#endif
