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

#include "EnumerateContentResult.h"
#include "EnumerateElementsResult.h"
#include "EnumerationState.h"
#include "InstructionAnalysisResult.h"
#include "llvm/Support/ErrorHandling.h"

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

}
}
