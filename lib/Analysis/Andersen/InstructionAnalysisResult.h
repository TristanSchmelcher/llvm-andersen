//===- InstructionAnalysisResult.h ----------------------------------------===//
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

#ifndef INSTRUCTIONANALYSISRESULT_H
#define INSTRUCTIONANALYSISRESULT_H

#include "AnalysisResult.h"
#include "llvm/ADT/OwningPtr.h"

#include <cassert>
#include <vector>

namespace llvm {
namespace andersen_internal {

class EnumerationState;
class ValueInfo;

class InstructionAnalysisResult : public AnalysisResult {
  OwningPtr<EnumerationState> ES;
  ValueInfo *ValueInfoContent;
  std::vector<AnalysisResult *> SubsetsContent;

public:
  InstructionAnalysisResult();
  virtual ~InstructionAnalysisResult();
  virtual EnumerateContentResult enumerateContent(size_t i, int Depth);

  // For INSTRUCTION_ANALYSIS_PHASE only.
  void setValueInfoContent(ValueInfo *VI) {
    assert(!ValueInfoContent);
    ValueInfoContent = VI;
  }

  // For INSTRUCTION_ANALYSIS_PHASE only.
  void addSubset(AnalysisResult *AR) {
    SubsetsContent.push_back(AR);
  }

  EnumerationState *getEnumerationState();
};

}
}

#endif
