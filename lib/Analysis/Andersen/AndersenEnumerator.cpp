//===- AndersenEnumerator.cpp - enumerator for AndersenPass ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an enumerator for the contents of lazy sets created by
// AndersenPass.
//
//===----------------------------------------------------------------------===//

#include "llvm/Analysis/AndersenEnumerator.h"

#include "EnumerateElementsResult.h"
#include "EnumerationState.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

using namespace andersen_internal;

ValueInfo *AndersenEnumerator::enumerate() {
  if (!ES) {
    // This means the empty set.
    return 0;
  }
  EnumerateElementsResult EER(ES->enumerateElements(i, 0));
  switch (EER.getResultType()) {
  case EnumerateElementsResult::COMPLETE:
    break;

  case EnumerateElementsResult::RETRY:
    llvm_unreachable("Received uncancelled retry-result");
    break;

  case EnumerateElementsResult::ELEMENT: 
    ++i;
    return EER.getElement();

  default:
    llvm_unreachable("Not a recognized EnumerationResult");
    break;
  }
  return 0;
}

}
