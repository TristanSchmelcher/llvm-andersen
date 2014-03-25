//===- CompositionWorkBase.h - analysis classes ---------------------------===//
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

#ifndef COMPOSITIONWORKBASE_H
#define COMPOSITIONWORKBASE_H

#include "TransformWork.h"

namespace llvm {
namespace andersen_internal {

class AlgorithmId;
class Constraints;
class AnalysisResult;
class ValueInfo;

class CompositionWorkBase : public TransformWork {
public:
  explicit CompositionWorkBase(AnalysisResult *AR);
  virtual ~CompositionWorkBase();
  virtual void writeFormula(const DebugInfo &DI, raw_ostream &OS) const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;

protected:
  virtual EnumerationResult transformValueInfo(EnumerationContext *Ctx,
      Constraints *C, ValueInfo *VI) const;
  virtual AnalysisResult *analyzeValueInfo(ValueInfo *VI) const = 0;
  virtual const AlgorithmId *getAlgorithmId() const = 0;
};

}
}

#endif
