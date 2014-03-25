//===- CompositionWorkBase.cpp - analysis classes -------------------------===//
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
#include "CompositionWorkBase.h"

#include "AlgorithmId.h"
#include "EnumerationContext.h"
#include "EnumerationResult.h"
#include "SubsetWork.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

CompositionWorkBase::CompositionWorkBase(AnalysisResult *AR)
  : TransformWork(AR) {}

CompositionWorkBase::~CompositionWorkBase() {}

EnumerationResult CompositionWorkBase::transformValueInfo(
    EnumerationContext *Ctx, Constraints *C, ValueInfo *VI) const {
  AnalysisResult *AR = analyzeValueInfo(VI);
  if (AR) {
    DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
                 << " In " << Ctx->getAnalysisResult() << ": transformed "
                 << E.getAnalysisResult() << '[' << (E.getPosition() - 1)
                 << "] to " << AR << '\n');
    SubsetWork *SW = Ctx->pushSubset(AR);
    if (SW) {
      EnumerationResult ER(SW->enumerate(Ctx, C));
      if (ER.getResultType() == EnumerationResult::COMPLETE) {
        Ctx->pop();
      }
      return ER;
    }
  }
  return EnumerationResult::makeCompleteResult();
}

void CompositionWorkBase::writeFormula(const DebugInfo &DI, raw_ostream &OS)
    const {
  getAlgorithmId()->printAlgorithmName(OS);
  OS << '(';
  E.writeFormula(DI, OS);
  OS << ')';
}

void CompositionWorkBase::printNodeLabel(const DebugInfo &DI, raw_ostream &OS)
    const {
  OS << "Transform(";
  getAlgorithmId()->printAlgorithmName(OS);
  OS << ')';
}

}
}
