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
typedef andersen_internal::ValueInfo *AndersenValueHandle;
typedef andersen_internal::AnalysisResult *AndersenSetHandle;

/// AndersenPass - An LLVM pass which implements Andersen's algorithm for
/// points-to analysis with some modifications for lazy evaluation.
class AndersenPass : public ModulePass {
  friend class AndersenGraphPass;
  andersen_internal::Data *Data;

public:
  static char ID; // Pass identification, replacement for typeid
  AndersenPass();

  // Get an opaque handle to the Andersen information for a given Value.
  AndersenValueHandle getHandleToValue(const Value *V) const;

  // Get an opaque handle to the set of regions that may be pointed-to by a
  // given value.
  static AndersenSetHandle getHandleToPointsToSet(AndersenValueHandle VH);

  // Get an opaque handle to the set of regions that may be read by a call to
  // a given function pointer.
  static AndersenSetHandle getHandleToFunctionPointerReadSet(
      AndersenValueHandle VH);

  // Get an opaque handle to the set of regions that may be written by a call to
  // a given function pointer.
  static AndersenSetHandle getHandleToFunctionPointerWriteSet(
      AndersenValueHandle VH);

  // Determine whether the set-intersection of two set handles is empty.
  static bool isSetIntersectionEmpty(AndersenSetHandle SH1,
      AndersenSetHandle SH2);

  // Determine whether a set handle is empty.
  static bool isSetEmpty(AndersenSetHandle SH);

  // Get an enumerator for the given set handle which computes the set lazily.
  static AndersenEnumerator enumerateSet(AndersenSetHandle SH);

private:
  virtual bool runOnModule(Module &M);
  virtual void releaseMemory();
  virtual void getAnalysisUsage(AnalysisUsage &AU) const;
  virtual void print(raw_ostream &OS, const Module * = 0) const;
};

}

#endif
