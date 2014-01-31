//===- TransformWorkBase.cpp - analysis classes ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a base type for transforms that compute the list
// comprehension of a ValueInfo-to-AnalysisResult function run on the elements
// of an input AnalysisResult.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "andersen"
#include "TransformWorkBase.h"

#include "AlgorithmId.h"
#include "EnumerationContext.h"
#include "EnumerationResult.h"
#include "SubsetWork.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>

namespace llvm {
namespace andersen_internal {

TransformWorkBase::TransformWorkBase(AnalysisResult *AR) : E(AR) {}

TransformWorkBase::~TransformWorkBase() {}

EnumerationResult TransformWorkBase::enumerate(EnumerationContext *Ctx) {
  for (;;) {
    DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                 << " In " << Ctx->getAnalysisResult() << ": transform "
                 << E.getAnalysisResult() << '[' << E.getPosition() << "]\n");
    EnumerationResult ER(E.enumerate(Ctx->getNextDepth(), Ctx->getDepth()));
    switch (ER.getResultType()) {
    case EnumerationResult::NEXT_VALUE: {
      AnalysisResult *AR = analyzeValueInfo(ER.getNextValue());
      if (!AR) {
        DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                     << " In " << Ctx->getAnalysisResult() << ": transformed "
                     << E.getAnalysisResult() << '[' << (E.getPosition() - 1)
                     << "] to empty set; continue\n");
        continue;
      }
      DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                   << " In " << Ctx->getAnalysisResult() << ": transformed "
                   << E.getAnalysisResult() << '[' << (E.getPosition() - 1)
                   << "] to " << AR << '\n');
      SubsetWork *SW = Ctx->pushSubset(AR);
      if (!SW) continue;
      return SW->enumerate(Ctx);
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

bool TransformWorkBase::prepareForRewrite(AnalysisResult *RewriteTarget) const {
  // Redundant transforms in the same AnalysisResult are not possible since each
  // distinct transform step is only ever created once, so we always splice the
  // transforms.
  return true;
}

void TransformWorkBase::writeFormula(const DebugInfo &DI, raw_ostream &OS)
    const {
  getAlgorithmId()->printAlgorithmName(OS);
  OS << '(';
  E.writeFormula(DI, OS);
  OS << ')';
}

GraphEdgeDeque TransformWorkBase::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  Result.push_back(E.toGraphEdge());
  return Result;
}

void TransformWorkBase::printNodeLabel(const DebugInfo &DI, raw_ostream &OS)
    const {
  OS << "Transform(";
  getAlgorithmId()->printAlgorithmName(OS);
  OS << ')';
}

}
}
