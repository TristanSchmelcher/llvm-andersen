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

#include "DebugInfo.h"
#include "EnumerateContentResult.h"
#include "EnumerationState.h"
#include "llvm/Support/raw_ostream.h"

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

void InstructionAnalysisResult::fillDebugInfo(DebugInfoFiller *DIF) const {
  EnumerationState *Ptr = ES.get();
  if (Ptr) {
    DIF->fill(Ptr, this);
  }
}

void InstructionAnalysisResult::writeEquation(const DebugInfo &DI,
    raw_ostream &OS) const {
  DI.printAnalysisResultName(this, OS);
  OS << " = ";
  bool first = true;
  if (ValueInfoContent) {
    OS << '{';
    DI.printValueInfoName(ValueInfoContent, OS);
    OS << '}';
    first = false;
  }
  for (std::vector<AnalysisResult *>::const_iterator i = SubsetsContent.begin();
       i != SubsetsContent.end(); ++i) {
    if (!first) {
      OS << " U ";
    }
    first = false;
    DI.printAnalysisResultName(*i, OS);
  }
  if (first) {
    // Empty.
    OS << "{}";
  }
  OS << '\n';
  EnumerationState *Ret = ES.get();
  if (Ret) {
    Ret->writeEquation(DI, OS);
  }  
}

EnumerationState *InstructionAnalysisResult::getEnumerationState() {
  EnumerationState *Ret = ES.get();
  if (!Ret) {
    Ret = new EnumerationState(this);
    ES.reset(Ret);
  }
  return Ret;
}

}
}
