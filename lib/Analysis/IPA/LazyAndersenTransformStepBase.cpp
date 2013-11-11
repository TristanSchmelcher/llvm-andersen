//===- LazyAndersenTransformStepBase.cpp - analysis classes ---------------===//
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

#include "LazyAndersenTransformStepBase.h"

#include "LazyAndersenEnumerationContext.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

TransformStepBase::TransformStepBase(AnalysisResult *AR) : E(AR) {}

TransformStepBase::~TransformStepBase() {}

EnumerationResult TransformStepBase::enumerate(EnumerationContext *Ctx) {
  for (;;) {
    EnumerationResult ER(E.enumerate(Ctx->getNextDepth()));
    switch (ER.getResultType()) {
    case EnumerationResult::NEXT_VALUE: {
      AnalysisResult *AR = analyzeValueInfo(ER.getNextValue());
      if (!AR) continue;
      return Ctx->pushWork(AR);
    }

    case EnumerationResult::RETRY:
    case EnumerationResult::COMPLETE:
      return ER;

    default:
      llvm_unreachable("Not a recognized EnumerationResult");
      break;
    }
    // Not reached.
    return ER;
  }
}

GraphEdgeDeque TransformStepBase::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  Result.push_back(E.toGraphEdge());
  return Result;
}
