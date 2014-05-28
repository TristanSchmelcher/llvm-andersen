//===- DropCallSitesTransformWork.h ---------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef DROPCALLSITESTRANSFORMWORK_H
#define DROPCALLSITESTRANSFORMWORK_H

#include "TransformWork.h"

namespace llvm {
namespace andersen_internal {

class AnalysisResult;

class DropCallSitesTransformWork : public TransformWork {
public:
  DropCallSitesTransformWork(AnalysisResult *AR);
  virtual ~DropCallSitesTransformWork();
  virtual void writeFormula(const DebugInfo &DI, raw_ostream &OS) const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;

protected:
  virtual EnumerationResult transformValueInfo(EnumerationContext *Ctx,
      Constraints *C, ValueInfo *VI) const;
};

}
}

#endif
