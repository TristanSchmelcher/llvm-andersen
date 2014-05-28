//===- TraversalAlgorithmId.cpp -------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an algorithm id type for algorithms whose names are built
// from the concatenation of two other algorithm names representing a traversal.
//
//===----------------------------------------------------------------------===//

#include "TraversalAlgorithmId.h"

#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

TraversalAlgorithmId::TraversalAlgorithmId(
    const AlgorithmId *SrcId, const AlgorithmId *DstId)
  : SrcId(SrcId), DstId(DstId) {}

TraversalAlgorithmId::~TraversalAlgorithmId() {}

void TraversalAlgorithmId::printAlgorithmName(raw_ostream &OS) const {
  DstId->printAlgorithmName(OS);
  // Using "o" to mean mathematical composition.
  OS << " o ";
  SrcId->printAlgorithmName(OS);
}

}
}
