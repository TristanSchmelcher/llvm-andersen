//===- RecursiveEnumerate.h - algorithm classes ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for recusively enumerating another AnalysisResult.
//
//===----------------------------------------------------------------------===//

#ifndef RECURSIVEENUMERATE_H
#define RECURSIVEENUMERATE_H

#include "AnalysisResultWork.h"
#include "llvm/Analysis/AndersenEnumerator.h"

namespace llvm {
namespace andersen_internal {
  class AnalysisResult;

  class RecursiveEnumerate : public AnalysisResultWork {
    Enumerator E;

  public:
    RecursiveEnumerate(AnalysisResult *AR);
    ~RecursiveEnumerate();
    virtual EnumerationResult enumerate(EnumerationContext *Ctx);
    virtual GraphEdgeDeque getOutgoingEdges() const;
    virtual std::string getNodeLabel(const Data &Data) const;
  };
}
}

#endif
