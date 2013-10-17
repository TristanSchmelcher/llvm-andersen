//===- LazyAndersenTopEnumerator.cpp - lazy set enumerator ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an enumerator for the points-to results.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenTopEnumerator.h"

#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

TopEnumerator::TopEnumerator(AnalysisResult *AR) : E(AR) {}

TopEnumerator::~TopEnumerator() {}

ValueInfo *TopEnumerator::enumerate() {
  AnalysisResult::EnumerationResult ER(E.enumerate(0));
  switch (ER.getResultType()) {
  case AnalysisResult::EnumerationResult::NEXT_VALUE:
    return ER.getNextValue();

  case AnalysisResult::EnumerationResult::RETRY:
    llvm_unreachable("Received uncancelled retry-result");
    break;

  case AnalysisResult::EnumerationResult::COMPLETE:
    return 0;

  default:
    llvm_unreachable("Not a recognized EnumerationResult");
    break;
  }
  return 0;
}
