//===- TransformWork.h - analysis classes ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a base type for transformations from one set to another
// set.
//
//===----------------------------------------------------------------------===//

#ifndef TRANSFORMWORK_H
#define TRANSFORMWORK_H

#include "AnalysisResultWork.h"
#include "Enumerator.h"

namespace llvm {
namespace andersen_internal {

class AnalysisResult;
class Constraints;
class ValueInfo;

class TransformWork : public AnalysisResultWork {
protected:
  Enumerator E;

public:
  explicit TransformWork(AnalysisResult *AR);
  virtual ~TransformWork();
  virtual EnumerationResult enumerate(EnumerationContext *Ctx, Constraints *C);
  virtual bool prepareForRewrite(AnalysisResult *RewriteTarget) const;
  virtual GraphEdgeDeque getOutgoingEdges() const;

protected:
  virtual EnumerationResult transformValueInfo(EnumerationContext *Ctx,
      Constraints *C, ValueInfo *VI) const = 0;
};

}
}

#endif
