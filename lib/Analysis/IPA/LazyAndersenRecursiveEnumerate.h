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

#include "LazyAndersenAnalysisResultPendingWorkEntry.h"

namespace llvm {
namespace lazyandersen {
  class RecursiveEnumerate : public AnalysisResultPendingWorkEntry {
    AnalysisResult::Enumerator E;

  public:
    RecursiveEnumerate(AnalysisResult *AR);
    ~RecursiveEnumerate();

    virtual AnalysisResult::EnumerationResult enumerate(
        AnalysisResult::Enumerator::Context *Ctx);
    virtual GraphEdgeDeque getOutgoingEdges() const;
    virtual std::string getNodeLabel() const;
  };
}
}

#endif
