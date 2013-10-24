//===- LazyAndersenAnalysisResult.h - algorithm classes -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULT_H
#define LAZYANDERSENANALYSISRESULT_H

#include "LazyAndersenAnalysisResultWork.h"
#include "LazyAndersenGraphNode.h"
#include "llvm/ADT/SetVector.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  class AnalysisResult : public GraphNode {
    friend class EnumerationContext;
    friend class Enumerator;
    friend class ScopedSetEnumerating;

    int EnumerationDepth;
    typedef SetVector<ValueInfo *> ValueInfoSetVector;
    ValueInfoSetVector Set;
    AnalysisResultWorkList Work;

  public:
    AnalysisResult();
    virtual ~AnalysisResult();

    bool addValueInfo(ValueInfo *VI) { return Set.insert(VI); }
    void addWork(AnalysisResultWork *Entry) { Work.push_back(Entry); }

    virtual GraphEdgeDeque getOutgoingEdges() const;
    virtual std::string getNodeLabel() const;
    virtual bool isNodeHidden() const;

  private:
    bool isEnumerating() const { return EnumerationDepth >= 0; }
  };
}
}

#endif
