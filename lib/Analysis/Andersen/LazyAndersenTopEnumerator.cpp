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

#include "LazyAndersenEnumerationResult.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::lazyandersen;

ValueInfo *TopEnumerator::enumerate() {
  EnumerationResult ER(E.enumerate(0, -1));
  switch (ER.getResultType()) {
  case EnumerationResult::NEXT_VALUE:
    return ER.getNextValue();

  case EnumerationResult::RETRY:
    llvm_unreachable("Received uncancelled retry-result");
    break;

  case EnumerationResult::REWRITE:
    llvm_unreachable("Received unfinished rewrite-result");
    break;

  case EnumerationResult::COMPLETE:
    return 0;

  default:
    llvm_unreachable("Not a recognized EnumerationResult");
    break;
  }
  // Not reached.
  return 0;
}
