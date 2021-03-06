//===- LiteralAlgorithmId.cpp ---------------------------------------------===//
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

#include "LiteralAlgorithmId.h"

#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

LiteralAlgorithmId::LiteralAlgorithmId(const char *Name) : Name(Name) {}

LiteralAlgorithmId::~LiteralAlgorithmId() {}

void LiteralAlgorithmId::printAlgorithmName(raw_ostream &OS) const {
  OS << Name;
}

}
}
