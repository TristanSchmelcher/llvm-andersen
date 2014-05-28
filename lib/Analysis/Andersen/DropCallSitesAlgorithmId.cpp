//===- DropCallSitesAlgorithmId.cpp ---------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "DropCallSitesAlgorithmId.h"

#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

DropCallSitesAlgorithmId::DropCallSitesAlgorithmId(
    const AlgorithmId *TransformedId)
  : TransformedId(TransformedId) {}

DropCallSitesAlgorithmId::~DropCallSitesAlgorithmId() {}

void DropCallSitesAlgorithmId::printAlgorithmName(raw_ostream &OS) const {
  OS << "drop-callsites(";
  TransformedId->printAlgorithmName(OS);
  OS << ')';
}

}
}
