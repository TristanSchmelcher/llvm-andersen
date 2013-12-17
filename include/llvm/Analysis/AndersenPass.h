//===- llvm/Analysis/AndersenPass.h - Andersen's algorithm ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// The AndersenPass class is an LLVM pass which implements Andersen's algorithm
// for points-to analysis with some modifications for lazy evaluation.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ANALYSIS_ANDERSENPASS_H
#define LLVM_ANALYSIS_ANDERSENPASS_H

#include "llvm/ADT/SetVector.h"
#include "llvm/Pass.h"

namespace llvm {
namespace andersen_internal {

class AnalysisResult;
class Data;
class ValueInfo;

}
}

namespace llvm {

class AndersenEnumerator;
class Value;
// Same as andersen_internal::ValueInfoSetVector.
typedef SetVector<andersen_internal::ValueInfo *> PointsToSet;

/// AndersenPass - An LLVM pass which implements Andersen's algorithm for
/// points-to analysis with some modifications for lazy evaluation.
class AndersenPass : public ModulePass {
  friend class AndersenGraphPass;
  andersen_internal::Data *Data;

public:
  static char ID; // Pass identification, replacement for typeid
  AndersenPass();

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
