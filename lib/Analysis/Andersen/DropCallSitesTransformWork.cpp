//===- DropCallSitesTransformWork.cpp -------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "DropCallSitesTransformWork.h"

#include "Constraints.h"
#include "EnumerationResult.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

DropCallSitesTransformWork::DropCallSitesTransformWork(AnalysisResult *AR)
  : TransformWork(AR) {}

DropCallSitesTransformWork::~DropCallSitesTransformWork() {}

EnumerationResult DropCallSitesTransformWork::transformValueInfo(
    EnumerationContext *Ctx, Constraints *C, ValueInfo *VI) const {
  C->CallStackContext.clear();
  return EnumerationResult::makeNextValueResult(VI);
}

void DropCallSitesTransformWork::writeFormula(const DebugInfo &DI,
    raw_ostream &OS) const {
  // TODO: Factor out duplicated code in CompositionWorkBase.
  OS << "drop-callsites(";
  E.writeFormula(DI, OS);
  OS << ')';
}

void DropCallSitesTransformWork::printNodeLabel(const DebugInfo &DI,
    raw_ostream &OS) const {
  OS << "Transform(drop-callsites)";
}

}
}
