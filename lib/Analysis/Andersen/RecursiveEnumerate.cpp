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

#include "EnumerationContext.h"
#include "EnumerationResult.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

RecursiveEnumerate::RecursiveEnumerate(AnalysisResult *AR) : E(AR) {}

RecursiveEnumerate::~RecursiveEnumerate() {}

EnumerationResult RecursiveEnumerate::enumerate(EnumerationContext *Ctx) {
  DEBUG(dbgs() << Ctx->getDepth() << ':' << Ctx->getLastTransformDepth()
               << " In " << Ctx->getAnalysisResult() << ": recurse to "
               << E.getAnalysisResult() << '[' << E.getPosition() << "]\n");
  return E.enumerate(Ctx->getNextDepth(), Ctx->getLastTransformDepth());
}

void RecursiveEnumerate::writeFormula(const Data &Data, raw_ostream &OS) const {
  E.writeFormula(Data, OS);
}

GraphEdgeDeque RecursiveEnumerate::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  Result.push_back(E.toGraphEdge());
  return Result;
}

std::string RecursiveEnumerate::getNodeLabel(const Data &Data) const {
  return "Recurse";
}

}
}
