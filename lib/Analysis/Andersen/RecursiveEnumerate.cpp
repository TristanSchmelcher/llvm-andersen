//===- RecursiveEnumerate.cpp - analysis classes --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for recusively enumerating another AnalysisResult.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "andersen"
#include "RecursiveEnumerate.h"

#include "AnalysisResult.h"
#include "EnumerationContext.h"
#include "EnumerationResult.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

RecursiveEnumerate::RecursiveEnumerate(AnalysisResult *AR) : E(AR) {}

RecursiveEnumerate::~RecursiveEnumerate() {}

EnumerationResult RecursiveEnumerate::enumerate(EnumerationContext *Ctx) {
  if (Ctx->canInline()) {
    DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                 << " In " << Ctx->getAnalysisResult() << ": inlining "
                 << E.getAnalysisResult() << '[' << E.getPosition() << ":]\n");
    return EnumerationResult::makeInlineResult(&E);
  }
  DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
               << " In " << Ctx->getAnalysisResult() << ": recurse to "
               << E.getAnalysisResult() << '[' << E.getPosition() << "]\n");
  for (;;) {
    EnumerationResult ER(E.enumerate(Ctx->getNextDepth(),
                                     Ctx->getLastTransformDepth()));
    if (ER.getResultType() != EnumerationResult::INLINE) {
      return ER;
    }

    const Enumerator &NewE(*ER.getInlineEnumerator());
    if (!Ctx->getAnalysisResult()->prepareForSubset(NewE.getAnalysisResult())) {
      DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                   << " In " << Ctx->getAnalysisResult()
                   << ": optimized away inline of " << NewE.getAnalysisResult()
                   << '[' << NewE.getPosition() << ":]\n");
      return EnumerationResult::makeCompleteResult();
    }
    E = NewE;
    DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                 << " In " << Ctx->getAnalysisResult() << ": inlined "
                 << NewE.getAnalysisResult() << '[' << NewE.getPosition()
                 << ":]\n");
  }
}

bool RecursiveEnumerate::prepareForRewrite(AnalysisResult *RewriteTarget)
    const {
  // If the target doesn't want this RecursiveEnumerate then delete it instead
  // of moving it.
  return RewriteTarget->prepareForSubset(E.getAnalysisResult());
}

void RecursiveEnumerate::writeFormula(const DebugInfo &DI,
    raw_ostream &OS) const {
  E.writeFormula(DI, OS);
}

GraphEdgeDeque RecursiveEnumerate::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  Result.push_back(E.toGraphEdge());
  return Result;
}

void RecursiveEnumerate::printNodeLabel(const DebugInfo &DI,
                                        raw_ostream &OS) const {
  OS << "Recurse";
}

}
}
