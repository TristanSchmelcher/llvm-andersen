//===- TransformAnalysisResult.h ------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef TRANSFORMANALYSISRESULT_H
#define TRANSFORMANALYSISRESULT_H

#include "AnalysisResult.h"

#include <vector>

namespace llvm {
namespace andersen_internal {

class EnumerationState;
class InstructionAnalysisResult;
class ValueInfo;

class TransformAnalysisResult : public AnalysisResult {
  EnumerationState *const ES;
  size_t j;
  std::vector<AnalysisResult *> SubsetsContent;

public:
  TransformAnalysisResult(InstructionAnalysisResult *IAR);
  virtual ~TransformAnalysisResult();
  virtual EnumerateContentResult enumerateContent(size_t i, int Depth);

private:
  virtual AnalysisResult *analyzeElement(ValueInfo *VI) const = 0;
};

}
}

#endif
