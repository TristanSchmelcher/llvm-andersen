//===- TransformAnalysisResult.cpp ----------------------------------------===//
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

#include "TransformAnalysisResult.h"

#include "AlgorithmId.h"
#include "DebugInfo.h"
#include "EnumerateContentResult.h"
#include "EnumerateElementsResult.h"
#include "EnumerationState.h"
#include "InstructionAnalysisResult.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

TransformAnalysisResult::TransformAnalysisResult(
    InstructionAnalysisResult *IAR) : ES(IAR->getEnumerationState()), j(0) {}

TransformAnalysisResult::~TransformAnalysisResult() {}

EnumerateContentResult TransformAnalysisResult::enumerateContent(size_t i,
    int Depth) {
  assert(i <= SubsetsContent.size());
  AnalysisResult *AR;
  if (i < SubsetsContent.size()) {
    AR = SubsetsContent[i];
  } else {
    do {
      EnumerateElementsResult EER(ES->enumerateElements(j, Depth));
      switch (EER.getResultType()) {
      case EnumerateElementsResult::COMPLETE:
        return EnumerateContentResult::makeCompleteResult();

      case EnumerateElementsResult::RETRY:
        return EnumerateContentResult::makeRetryResult(
            EER.getRetryCancellationPoint());

      case EnumerateElementsResult::ELEMENT:
        ++j;
        AR = analyzeElement(EER.getElement());
        break;

      default:
        llvm_unreachable("Not a recognized EnumerateElementsResult");
        break;
      }
    } while (!AR);
    SubsetsContent.push_back(AR);
  }
  return EnumerateContentResult::makeSubsetContentResult(AR);
}

void TransformAnalysisResult::fillDebugInfo(DebugInfoFiller *DIF) const {}

void TransformAnalysisResult::writeEquation(const DebugInfo &DI,
    raw_ostream &OS) const {
  DI.printAnalysisResultName(this, OS);
  OS << " = ";
  bool first = true;
  for (std::vector<AnalysisResult *>::const_iterator i = SubsetsContent.begin();
       i != SubsetsContent.end(); ++i) {
    if (!first) {
      OS << " U ";
    }
    first = false;
    DI.printAnalysisResultName(*i, OS);
  }
  if (!first) {
    OS << " U ";
  }
  getAlgorithmId()->printAlgorithmName(OS);
  OS << '(';
  DI.printEnumerationStateName(ES, OS);
  OS << '[' << j << ":])\n";
}

}
}
