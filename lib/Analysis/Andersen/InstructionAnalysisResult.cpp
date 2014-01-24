//===- InstructionAnalysisResult.cpp --------------------------------------===//
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

#include "InstructionAnalysisResult.h"

#include "EnumerateContentResult.h"
#include "EnumerationState.h"

namespace llvm {
namespace andersen_internal {

InstructionAnalysisResult::InstructionAnalysisResult() : ValueInfoContent(0) {}

InstructionAnalysisResult::~InstructionAnalysisResult() {}

EnumerateContentResult InstructionAnalysisResult::enumerateContent(size_t i,
    int Depth) {
  if (ValueInfoContent) {
    if (i == 0) {
      return EnumerateContentResult::makeValueInfoContentResult(
          ValueInfoContent);
    } else {
      --i;
    }
  }
  assert(i <= SubsetsContent.size());
  if (i >= SubsetsContent.size()) {
    return EnumerateContentResult::makeCompleteResult();
  }
  return EnumerateContentResult::makeSubsetContentResult(SubsetsContent[i]);
}

EnumerationState *InstructionAnalysisResult::getEnumerationState() {
  EnumerationState *Ret;
  if (!(Ret = ES.get())) {
    Ret = new EnumerationState(this);
    ES.reset(Ret);
  }
  return Ret;
}

}
}
