//===- TransformStepBase.h - analysis classes -----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a base type for transforms that compute the list
// comprehension of a ValueInfo-to-AnalysisResult function run on the elements
// of an input AnalysisResult.
//
//===----------------------------------------------------------------------===//

#ifndef TRANSFORMSTEPBASE_H
#define TRANSFORMSTEPBASE_H

#include "AnalysisResultWork.h"
#include "Enumerator.h"

#include <string>

namespace llvm {
namespace andersen_internal {

class AlgorithmId;
class AnalysisResult;
class ValueInfo;

class TransformStepBase : public AnalysisResultWork {
  Enumerator E;

public:
  explicit TransformStepBase(AnalysisResult *AR);
  virtual ~TransformStepBase();
  virtual EnumerationResult enumerate(EnumerationContext *Ctx);
  virtual bool prepareForRewrite(AnalysisResult *RewriteTarget) const;
  virtual void writeFormula(const DebugInfo &DI, raw_ostream &OS) const;
  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;

protected:
  virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) = 0;
  virtual const AlgorithmId *getAlgorithmId() const = 0;
};

}
}

#endif
