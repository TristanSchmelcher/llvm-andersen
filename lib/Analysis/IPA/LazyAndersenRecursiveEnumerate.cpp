//===- LazyAndersenRecursiveEnumerate.cpp - analysis classes --------------===//
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

#include "LazyAndersenRecursiveEnumerate.h"

using namespace llvm;
using namespace llvm::lazyandersen;

RecursiveEnumerate::RecursiveEnumerate(AnalysisResult *AR) : E(AR) {}

RecursiveEnumerate::~RecursiveEnumerate() {}

AnalysisResult::EnumerationResult RecursiveEnumerate::enumerate(
    AnalysisResult *Owner,
    AnalysisResultEntryBaseList::iterator *j,
    int Depth) {
  return E.enumerate(Depth);
}

GraphEdgeDeque RecursiveEnumerate::getOutgoingEdges() const {
  // TODO
  return GraphEdgeDeque();
}

std::string RecursiveEnumerate::getNodeLabel() const {
  return "Recurse";
}
