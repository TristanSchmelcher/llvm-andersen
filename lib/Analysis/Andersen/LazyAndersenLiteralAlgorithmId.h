//===- LazyAndersenLiteralAlgorithmId.h -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares an algorithm id type for algorithms whose names are string
// literal.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENLITERALALGORITHMID_H
#define LAZYANDERSENLITERALALGORITHMID_H

#include "LazyAndersenAlgorithmId.h"

namespace llvm {
namespace lazyandersen {
  class LiteralAlgorithmId : public AlgorithmId {
  public:
    LiteralAlgorithmId(const char *Name);
    virtual ~LiteralAlgorithmId();
    virtual std::string getAlgorithmName() const;

  private:
    const char *const Name;
  };
}
}

#endif
