//===- TransformStepBase.cpp - analysis classes ---------------------------===//
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
#include "TransformStepBase.h"

#include "AlgorithmId.h"
#include "EnumerationContext.h"
#include "EnumerationResult.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>

namespace llvm {
namespace andersen_internal {

TransformStepBase::TransformStepBase(AnalysisResult *AR) : E(AR) {}

TransformStepBase::~TransformStepBase() {}

EnumerationResult TransformStepBase::enumerate(EnumerationContext *Ctx) {
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
      return Ctx->pushWork(AR);
    }

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

void TransformStepBase::writeFormula(const DebugInfo &DI,
    raw_ostream &OS) const {
  getAlgorithmId()->printAlgorithmName(OS);
  OS << '(';
  E.writeFormula(DI, OS);
  OS << ')';
}

GraphEdgeDeque TransformStepBase::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  Result.push_back(E.toGraphEdge());
  return Result;
}

void TransformStepBase::printNodeLabel(const DebugInfo &DI,
    raw_ostream &OS) const {
  OS << "Transform(";
  getAlgorithmId()->printAlgorithmName(OS);
  OS << ')';
}

}
}
