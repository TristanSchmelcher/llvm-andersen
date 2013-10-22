//===- LazyAndersenMetaAnalysisStepBase.cpp - analysis classes ------------===//
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

#include "LazyAndersenAnalysisResult.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

MetaAnalysisStep::MetaAnalysisStep(AnalysisResult *AR) : E(AR) {}

MetaAnalysisStep::~MetaAnalysisStep() {}

AnalysisResult::EnumerationResult MetaAnalysisStep::enumerate(
    AnalysisResult::Enumerator::Context *Ctx) {
  AnalysisResult::EnumerationResult ER(E.enumerate(Ctx->getNextDepth()));
  switch (ER.getResultType()) {
  case AnalysisResult::EnumerationResult::NEXT_VALUE:
    return Ctx->pushWork(analyzeValueInfo(ER.getNextValue()));

  case AnalysisResult::EnumerationResult::RETRY:
  case AnalysisResult::EnumerationResult::COMPLETE:
    return ER;

  default:
    llvm_unreachable("Not a recognized EnumerationResult");
    break;
  }
  // Not reached.
  return AnalysisResult::EnumerationResult::makeCompleteResult();
}

GraphEdgeDeque MetaAnalysisStep::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  Result.push_back(E.toGraphEdge());
  return Result;
}
