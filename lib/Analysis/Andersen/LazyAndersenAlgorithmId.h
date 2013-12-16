//===- LazyAndersenAlgorithmId.h ------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares an interface for algorithm ids.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENALGORITHMID_H
#define LAZYANDERSENALGORITHMID_H

#include <string>

namespace llvm {
namespace andersen_internal {
  class AlgorithmId {
  public:
    virtual std::string getAlgorithmName() const = 0;

  protected:
    ~AlgorithmId() {}
  };
}
}

#endif
