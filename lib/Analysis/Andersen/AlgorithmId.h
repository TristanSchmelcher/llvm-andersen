//===- AlgorithmId.h ------------------------------------------------------===//
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

#ifndef ALGORITHMID_H
#define ALGORITHMID_H

namespace llvm {

class raw_ostream;

}


namespace llvm {
namespace andersen_internal {

class AlgorithmId {
public:
  virtual ~AlgorithmId() {}
  virtual void printAlgorithmName(raw_ostream &OS) const = 0;
};

}
}

#endif
