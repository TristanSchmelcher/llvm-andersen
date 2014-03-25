//===- TransformWork.cpp - analysis classes -------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a base type for transformations from one set to another
// set.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "andersen"
#include "TransformWork.h"

#include "EnumerationContext.h"
#include "EnumerationResult.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

TransformWork::TransformWork(AnalysisResult *AR) : E(AR) {}

TransformWork::~TransformWork() {}

EnumerationResult TransformWork::enumerate(EnumerationContext *Ctx,
    Constraints *C) {
  for (;;) {
    DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                 << " In " << Ctx->getAnalysisResult() << ": transform "
                 << E.getAnalysisResult() << '[' << E.getPosition() << "]\n");
    EnumerationResult ER(E.enumerate(Ctx->getNextDepth(),
                                     Ctx->getDepth(),
                                     C));
    switch (ER.getResultType()) {
    case EnumerationResult::NEXT_VALUE: {
      ValueInfo *VI = ER.getNextValue();
      ER = transformValueInfo(Ctx, C, VI);
      if (ER.getResultType() == EnumerationResult::COMPLETE) {
        DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                     << " In " << Ctx->getAnalysisResult() << ": transformed "
                     << E.getAnalysisResult() << '[' << (E.getPosition() - 1)
                     << "] to empty set; continue\n");
        continue;
      }
      break;
    }

    case EnumerationResult::INLINE:
      llvm_unreachable("Cannot inline past a transform");
      break;

    case EnumerationResult::RETRY:
    case EnumerationResult::COMPLETE:
      break;

    case EnumerationResult::REWRITE:
      llvm_unreachable("Cannot rewrite past a transform");
      break;

    default:
      llvm_unreachable("Not a recognized EnumerationResult");
      break;
    }
    return ER;
  }
}

bool TransformWork::prepareForRewrite(AnalysisResult *RewriteTarget) const {
  // Redundant transforms in the same AnalysisResult are not possible since each
  // distinct transform step is only ever created once, so we always splice the
  // transforms.
  return true;
}

GraphEdgeDeque TransformWork::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  Result.push_back(E.toGraphEdge());
  return Result;
}

}
}
