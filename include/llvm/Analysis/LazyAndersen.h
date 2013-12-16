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
namespace andersen_internal {

class AnalysisResult;
class LazyAndersenData;
class LazyAndersenGraphPass;
class ValueInfo;

}
}

namespace llvm {

class AndersenEnumerator;
class Value;
// Same as andersen_internal::ValueInfoSetVector.
typedef SetVector<andersen_internal::ValueInfo *> PointsToSet;

/// LazyAndersen - An LLVM pass which implements a lazy version of Andersen's
/// algorithm for points-to analysis.
class LazyAndersen : public ModulePass {
  friend class andersen_internal::LazyAndersenGraphPass;
  andersen_internal::LazyAndersenData *Data;

public:
  static char ID; // Pass identification, replacement for typeid
  LazyAndersen();

  // Get the points-to set of V, or null if V cannot point to anything. If the
  // points-to set has not yet been fully computed, this method computes it.
  // The elements should be treated as opaque ids for abstract memory regions in
  // the program.
  const PointsToSet *getPointsToSet(const Value *V) const;

  // Get an enumerator for the points-to set of V which computes the set
  // lazily. This is more efficient than getPointsToSet() if the full set may
  // not be needed.
  AndersenEnumerator enumeratePointsToSet(const Value *V) const;

private:
  andersen_internal::AnalysisResult *getPointsToSetAnalysisResult(
      const Value *V) const;

  virtual bool runOnModule(Module &M);
  virtual void releaseMemory();
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual void print(raw_ostream &OS, const Module * = 0) const;
};

}

#endif
