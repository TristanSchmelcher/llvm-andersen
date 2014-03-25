//===- PushCallSiteTransformWork.h ----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// PushCallSiteTransformWork transforms the values of an AnalysisResult by
// pushing a callsite constraint.
//
//===----------------------------------------------------------------------===//

#ifndef PUSHCALLSITETRANSFORMWORK_H
#define PUSHCALLSITETRANSFORMWORK_H

#include "TransformWork.h"

namespace llvm {

class Instruction;

}

namespace llvm {
namespace andersen_internal {

class AnalysisResult;

class PushCallSiteTransformWork : public TransformWork {
  const Instruction *const CallSite;

public:
  PushCallSiteTransformWork(AnalysisResult *AR, const Instruction *CallSite);

protected:
  virtual EnumerationResult transformValueInfo(EnumerationContext *Ctx,
      Constraints *C, ValueInfo *VI) const;
};

}
}

#endif
