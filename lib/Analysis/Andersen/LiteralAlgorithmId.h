//===- LiteralAlgorithmId.h -----------------------------------------------===//
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

#ifndef LITERALALGORITHMID_H
#define LITERALALGORITHMID_H

#include "AlgorithmId.h"

namespace llvm {
namespace andersen_internal {

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
