//===- LazyAndersenRecursiveEnumerate.h - algorithm classes ---------------===//
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

#ifndef LAZYANDERSENRECURSIVEENUMERATE_H
#define LAZYANDERSENRECURSIVEENUMERATE_H

#include "LazyAndersenAnalysisResultWork.h"
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
    virtual std::string getNodeLabel(const LazyAndersenData &Data) const;
  };
}
}

#endif