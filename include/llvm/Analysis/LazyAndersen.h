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

#include "llvm/ADT/SetVector.h"
#include "llvm/Pass.h"

namespace llvm {
namespace lazyandersen {

class AnalysisResult;
class LazyAndersenData;
class LazyAndersenGraphPass;
class TopEnumerator;
class ValueInfo;
typedef SetVector<ValueInfo *> ValueInfoSetVector;

}
}

namespace llvm {

class Value;

/// LazyAndersen - An LLVM pass which implements a lazy version of Andersen's
/// algorithm for points-to analysis.
class LazyAndersen : public ModulePass {
  friend class lazyandersen::LazyAndersenGraphPass;
  lazyandersen::LazyAndersenData *Data;

public:
  static char ID; // Pass identification, replacement for typeid
  LazyAndersen();

  // Get the points-to set of V, or null if V cannot point to anything. The
  // elements should be treated as opaque ids for abstract memory regions in the
  // program.
  const lazyandersen::ValueInfoSetVector *getPointsToSet(const Value *V) const;

  // Get an enumerator for the points-to set of V which enumerates the set
  // lazily. This is more efficient than getPointsToSet() if the full set may
  // not be needed.
  // TODO: Put TopEnumerator in a public header.
  lazyandersen::TopEnumerator enumeratePointsToSet(const Value *V) const;

private:
  lazyandersen::AnalysisResult *getPointsToSetAnalysisResult(const Value *V)
      const;

  virtual bool runOnModule(Module &M);
  virtual void releaseMemory();
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual void print(raw_ostream &OS, const Module * = 0) const;
};

}

#endif
