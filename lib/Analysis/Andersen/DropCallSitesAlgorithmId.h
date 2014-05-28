//===- DropCallSitesAlgorithmId.h -----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef DROPCALLSITESALGORITHMID_H
#define DROPCALLSITESALGORITHMID_H

#include "AlgorithmId.h"

namespace llvm {
namespace andersen_internal {

class DropCallSitesAlgorithmId : public AlgorithmId {
public:
  DropCallSitesAlgorithmId(const AlgorithmId *TransformedId);
  ~DropCallSitesAlgorithmId();

  virtual void printAlgorithmName(raw_ostream &OS) const;

private:
  const AlgorithmId *const TransformedId;
};

}
}

#endif
