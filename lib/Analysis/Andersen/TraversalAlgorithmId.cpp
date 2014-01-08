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

#include <sstream>

namespace llvm {
namespace andersen_internal {

TraversalAlgorithmId::TraversalAlgorithmId(
    const AlgorithmId *SrcId, const AlgorithmId *DstId)
  : SrcId(SrcId), DstId(DstId) {}

TraversalAlgorithmId::~TraversalAlgorithmId() {}

std::string TraversalAlgorithmId::getAlgorithmName() const {
  std::ostringstream OSS;
  // Using "o" to mean mathematical composition.
  OSS << DstId->getAlgorithmName() << " o " << SrcId->getAlgorithmName();
  return OSS.str();
}

}
}
