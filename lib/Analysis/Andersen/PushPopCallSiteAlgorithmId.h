//===- PushPopCallSiteAlgorithmId.h ---------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef PUSHPOPCALLSITEALGORITHMID_H
#define PUSHPOPCALLSITEALGORITHMID_H

#include "AlgorithmId.h"
#include "PushPopOperationType.h"

namespace llvm {

class Instruction;

}

namespace llvm {
namespace andersen_internal {

class PushPopCallSiteAlgorithmId : public AlgorithmId {
public:
  PushPopCallSiteAlgorithmId(PushPopOperationType Op,
      const Instruction *CallSite, const AlgorithmId *TransformedId);
  ~PushPopCallSiteAlgorithmId();

  virtual void printAlgorithmName(raw_ostream &OS) const;

  const Instruction *getCallSite() const { return CallSite; }

private:
  const PushPopOperationType Op;
  const Instruction *const CallSite;
  const AlgorithmId *const TransformedId;
};

}
}

#endif
