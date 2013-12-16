//===- LazyAndersenTransformStepBase.h - analysis classes ------------------===//
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

#ifndef LAZYANDERSENTRANSFORMSTEPBASE_H
#define LAZYANDERSENTRANSFORMSTEPBASE_H

#include "LazyAndersenAnalysisResultWork.h"
#include "llvm/Analysis/AndersenEnumerator.h"

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
    virtual GraphEdgeDeque getOutgoingEdges() const;
    virtual EnumerationResult enumerate(EnumerationContext *Ctx);

  protected:
    static std::string buildTransformStepName(const AlgorithmId& Id);

    virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) = 0;
  };
}
}

#endif
