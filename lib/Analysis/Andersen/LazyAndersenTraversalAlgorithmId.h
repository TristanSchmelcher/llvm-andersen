//===- LazyAndersenTraversalAlgorithmId.h ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares an algorithm id type for algorithms whose names are built
// from the concatenation of two other algorithm names representing a traversal.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENTRAVERSALALGORITHMID_H
#define LAZYANDERSENTRAVERSALALGORITHMID_H

#include "LazyAndersenAlgorithmId.h"

#include <string>

namespace llvm {
namespace andersen_internal {
  class TraversalAlgorithmId : public AlgorithmId {
  public:
    TraversalAlgorithmId(const AlgorithmId *SrcId, const AlgorithmId *DstId);
    ~TraversalAlgorithmId();

    virtual std::string getAlgorithmName() const;

  private:
    const AlgorithmId *const SrcId;
    const AlgorithmId *const DstId;
  };
}
}

#endif
