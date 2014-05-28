//===- PushPopCallSiteAlgorithmId.cpp -------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "PushPopCallSiteAlgorithmId.h"

#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

PushPopCallSiteAlgorithmId::PushPopCallSiteAlgorithmId(PushPopOperationType Op,
    const Instruction *CallSite, const AlgorithmId *TransformedId)
  : Op(Op), CallSite(CallSite), TransformedId(TransformedId) {}

PushPopCallSiteAlgorithmId::~PushPopCallSiteAlgorithmId() {}

void PushPopCallSiteAlgorithmId::printAlgorithmName(raw_ostream &OS) const {
  OS << ((Op == PUSH) ? "push" : "pop") << "-callsite[" << CallSite << "](";
  TransformedId->printAlgorithmName(OS);
  OS << ')';
}

}
}
