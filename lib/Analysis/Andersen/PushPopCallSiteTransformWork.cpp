//===- PushPopCallSiteTransformWork.cpp -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "PushPopCallSiteTransformWork.h"

#include "Constraints.h"
#include "EnumerationResult.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

template<PushPopOperationType Op>
PushPopCallSiteTransformWork<Op>::PushPopCallSiteTransformWork(
    AnalysisResult *AR, const Instruction *CallSite)
  : TransformWork(AR), CallSite(CallSite) {}

template<PushPopOperationType Op>
PushPopCallSiteTransformWork<Op>::~PushPopCallSiteTransformWork() {}

template<>
EnumerationResult PushPopCallSiteTransformWork<PUSH>::transformValueInfo(
    EnumerationContext *Ctx, Constraints *C, ValueInfo *VI) const {
  C->CallStackContext.push_back(CallSite);
  return EnumerationResult::makeNextValueResult(VI);
}

template<>
EnumerationResult PushPopCallSiteTransformWork<POP>::transformValueInfo(
    EnumerationContext *Ctx, Constraints *C, ValueInfo *VI) const {
  if (!C->CallStackContext.empty()) {
    const Instruction *LastCallSite = C->CallStackContext.back();
    if (LastCallSite != CallSite) {
      // Filter.
      return EnumerationResult::makeCompleteResult();
    }
    C->CallStackContext.pop_back();
  }
  return EnumerationResult::makeNextValueResult(VI);
}

template<PushPopOperationType Op>
void PushPopCallSiteTransformWork<Op>::writeFormula(const DebugInfo &DI,
    raw_ostream &OS) const {
  // TODO: Factor out duplicated code in CompositionWorkBase.
  OS << ((Op == PUSH) ? "push" : "pop") << "-callsite[" << CallSite << "](";
  E.writeFormula(DI, OS);
  OS << ')';
}

template<PushPopOperationType Op>
void PushPopCallSiteTransformWork<Op>::printNodeLabel(const DebugInfo &DI,
    raw_ostream &OS) const {
  OS << "Transform(" << ((Op == PUSH) ? "push" : "pop") << "-callsite["
     << CallSite << "])";
}

template class PushPopCallSiteTransformWork<PUSH>;
template class PushPopCallSiteTransformWork<POP>;

}
}
