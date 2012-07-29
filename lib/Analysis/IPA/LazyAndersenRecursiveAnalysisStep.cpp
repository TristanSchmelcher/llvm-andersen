//===- LazyAndersenRecursiveAnalysisStep.cpp - analysis classes -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for recursive analysis.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenRecursiveAnalysisStep.h"

using namespace llvm;
using namespace llvm::lazyandersen;

RecursiveAnalysisStep::RecursiveAnalysisStep(AnalysisResult *Inner)
  : Inner(Inner) {}

RecursiveAnalysisStep::Result RecursiveAnalysisStep::run() {
  // TODO
  return RecursiveAnalysisStep::Result();
}
