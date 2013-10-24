//===- LazyAndersenMetaAnalysisStep.cpp - analysis classes ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for meta analysis--i.e., analysis of other analysis
// results.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenMetaAnalysisStep.h"

#include "LazyAndersenEnumerationContext.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

MetaAnalysisStep::MetaAnalysisStep(AnalysisResult *AR) : E(AR) {}

MetaAnalysisStep::~MetaAnalysisStep() {}

EnumerationResult MetaAnalysisStep::enumerate(EnumerationContext *Ctx) {
  EnumerationResult ER(E.enumerate(Ctx->getNextDepth()));
  switch (ER.getResultType()) {
  case EnumerationResult::NEXT_VALUE:
    return Ctx->pushWork(analyzeValueInfo(ER.getNextValue()));

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

GraphEdgeDeque MetaAnalysisStep::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  Result.push_back(E.toGraphEdge());
  return Result;
}
