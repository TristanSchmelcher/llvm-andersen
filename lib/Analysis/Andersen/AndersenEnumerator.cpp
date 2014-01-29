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

#define DEBUG_TYPE "andersen"
#include "llvm/Analysis/AndersenEnumerator.h"

#include "AnalysisResult.h"
#include "EnumerationResult.h"
#include "Enumerator.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>

namespace llvm {
namespace andersen_internal {

EnumeratorState::EnumeratorState(AnalysisResult *AR, size_t i) : AR(AR), i(i) {
  assert(AR);
  assert(i <= AR->getSetContentsSoFar().size());
}

}
}

namespace llvm {

using namespace andersen_internal;

ValueInfo *AndersenEnumerator::enumerate() {
  Enumerator &E(Enumerator::forState(ES));
  DEBUG(dbgs() << "Begin " << E.getAnalysisResult() << '[' << E.getPosition()
               << "]\n");
  EnumerationResult ER(E.enumerate(0, -1));
  switch (ER.getResultType()) {
  case EnumerationResult::NEXT_VALUE:
    DEBUG(dbgs() << "Result: " << ER.getNextValue() << '\n'); 
    return ER.getNextValue();

  case EnumerationResult::INLINE:
    llvm_unreachable("Received spurious inline-result");
    break;

  case EnumerationResult::RETRY:
    llvm_unreachable("Received uncancelled retry-result");
    break;

  case EnumerationResult::REWRITE:
    llvm_unreachable("Received unfinished rewrite-result");
    break;

  case EnumerationResult::COMPLETE:
    DEBUG(dbgs() << "Result: end of set\n"); 
    break;

  default:
    llvm_unreachable("Not a recognized EnumerationResult");
    break;
  }
  return 0;
}

}
