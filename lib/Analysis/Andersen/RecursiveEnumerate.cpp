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

#include "RecursiveEnumerate.h"

#include "EnumerationContext.h"
#include "EnumerationResult.h"

namespace llvm {
namespace andersen_internal {

RecursiveEnumerate::RecursiveEnumerate(AnalysisResult *AR) : E(AR) {}

RecursiveEnumerate::~RecursiveEnumerate() {}

EnumerationResult RecursiveEnumerate::enumerate(EnumerationContext *Ctx) {
  return E.enumerate(Ctx->getNextDepth(), Ctx->getLastTransformDepth());
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
