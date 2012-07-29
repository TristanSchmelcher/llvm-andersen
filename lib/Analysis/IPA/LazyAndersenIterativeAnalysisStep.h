//===- LazyAndersenIterativeAnalysisStep.h - analysis classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for iterative analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENITERATIVEANALYSISSTEP_H
#define LAZYANDERSENITERATIVEANALYSISSTEP_H

#include "LazyAndersenAnalysisStep.h"

namespace llvm {
namespace lazyandersen {
  class IterativeAnalysisStep : public AnalysisStep {
  protected:
    // Push the next iteration onto the step list and return it as the result.
    Result nextIteration(AnalysisStep *Step);
    // Pop this completed step off the stack (thus deleting it) and return the
    // next one.
    Result done();
  };
}
}

#endif
