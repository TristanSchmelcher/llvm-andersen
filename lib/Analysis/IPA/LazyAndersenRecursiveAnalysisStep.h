//===- LazyAndersenRecursiveAnalysisStep.h - analysis classes -------------===//
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

#ifndef LAZYANDERSENRECURSIVEANALYSISSTEP_H
#define LAZYANDERSENRECURSIVEANALYSISSTEP_H

#include "LazyAndersenAnalysisStep.h"

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;

  class RecursiveAnalysisStep : public AnalysisStep {
    // We assume here that--once created--each AnalysisResult will persist
    // for the lifetime of the LazyAndersenData. If not, we'll need weak
    // pointers.
    AnalysisResult *Inner;

  public:
    RecursiveAnalysisStep(AnalysisResult *Inner);
    // TODO: Need a way to track which inner member is next. Perhaps run
    // needs to take a context? Or we could have a stack.
    virtual Result run();
  };
}
}

#endif
