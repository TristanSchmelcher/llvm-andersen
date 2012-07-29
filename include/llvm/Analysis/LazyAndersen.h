//===- llvm/Analysis/LazyAndersen.h - Lazy Andersen's algorithm -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// The LazyAndersen class is an LLVM pass which implements a lazy version of
// Andersen's algorithm for points-to analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ANALYSIS_LAZYANDERSEN_H
#define LLVM_ANALYSIS_LAZYANDERSEN_H

#include "llvm/Pass.h"

namespace llvm {
  class LazyAndersenData;

  /// LazyAndersen - An LLVM pass which implements a lazy version of Andersen's
  /// algorithm for points-to analysis.
  class LazyAndersen : public ModulePass {
    LazyAndersenData *Data;

  public:
    static char ID; // Pass identification, replacement for typeid
    LazyAndersen();

  private:
    virtual bool runOnModule(Module &M);
    virtual void releaseMemory();
    virtual void getAnalysisUsage(AnalysisUsage &AU) const;
    virtual void print(raw_ostream &OS, const Module* = 0) const;
  };
}

#endif
