//===- PushPopCallSiteTransformAlgorithm.h --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef PUSHPOPCALLSITETRANSFORMALGORITHM_H
#define PUSHPOPCALLSITETRANSFORMALGORITHM_H

#include "AnalysisResult.h"
#include "Data.h"
#include "IsNotNecessarilyEmptyIfMissingProperty.h"
#include "Phase.h"
#include "PushPopCallSiteAlgorithmId.h"
#include "PushPopCallSiteTransformWork.h"
#include "PushPopOperationType.h"
#include "ValueInfo.h"

#include <utility>

namespace llvm {
namespace andersen_internal {

template<typename FirstHopAlgorithm, PushPopOperationType Op>
class PushPopCallSiteTransformAlgorithm :
    public IsNotNecessarilyEmptyIfMissingProperty {
  // Unique address for this instantiation of this class. (The value stored in
  // the address is not used.)
  static const char ClassID;

public:
  const PushPopCallSiteAlgorithmId &ID;

  PushPopCallSiteTransformAlgorithm(Data *D, const Instruction *CallSite)
    : ID(*getAlgorithmId(D, CallSite)) {}

  AnalysisResult *run(ValueInfo *VI) const {
    AnalysisResult *AR = new AnalysisResult();
    AR->appendUniqueTransform(new PushPopCallSiteTransformWork<Op>(
        VI->getAlgorithmResult<INSTRUCTION_ANALYSIS_PHASE>(FirstHopAlgorithm()),
        ID.getCallSite()));
    return AR;
  }

private:
  static PushPopCallSiteAlgorithmId *getAlgorithmId(Data *D,
      const Instruction *CallSite) {
    PushPopCallSiteAlgorithmId *&Id(D->PushPopCallSiteAlgorithmIds[
        std::make_pair(&ClassID, CallSite)]);
    if (!Id) {
      Id = new PushPopCallSiteAlgorithmId(Op, CallSite, &FirstHopAlgorithm::ID);
    }
    return Id;
  }
};

template<typename FirstHopAlgorithm, PushPopOperationType Op>
const char PushPopCallSiteTransformAlgorithm<FirstHopAlgorithm, Op>::ClassID =
    0;

}
}

#endif
