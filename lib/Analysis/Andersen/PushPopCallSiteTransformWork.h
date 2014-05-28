//===- PushPopCallSiteTransformWork.h -------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef PUSHPOPCALLSITETRANSFORMWORK_H
#define PUSHPOPCALLSITETRANSFORMWORK_H

#include "PushPopOperationType.h"
#include "TransformWork.h"

namespace llvm {

class Instruction;

}

namespace llvm {
namespace andersen_internal {

class AnalysisResult;

template<PushPopOperationType Op>
class PushPopCallSiteTransformWork : public TransformWork {
  const Instruction *const CallSite;

public:
  PushPopCallSiteTransformWork(AnalysisResult *AR, const Instruction *CallSite);
  virtual ~PushPopCallSiteTransformWork();
  virtual void writeFormula(const DebugInfo &DI, raw_ostream &OS) const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;

protected:
  virtual EnumerationResult transformValueInfo(EnumerationContext *Ctx,
      Constraints *C, ValueInfo *VI) const;
};

template<>
EnumerationResult PushPopCallSiteTransformWork<PUSH>::transformValueInfo(
    EnumerationContext *Ctx, Constraints *C, ValueInfo *VI) const;

template<>
EnumerationResult PushPopCallSiteTransformWork<POP>::transformValueInfo(
    EnumerationContext *Ctx, Constraints *C, ValueInfo *VI) const;

}
}

#endif
