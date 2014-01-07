//===- AnalysisResult.h - algorithm classes -------------------------------===//
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

#ifndef ANALYSISRESULT_H
#define ANALYSISRESULT_H

#include "AnalysisResultWork.h"
#include "GraphNode.h"
#include "llvm/ADT/SetVector.h"

#include <string>

namespace llvm {
namespace andersen_internal {

class AlgorithmId;
class AnalysisResult;
class Data;
class ValueInfo;
typedef SetVector<ValueInfo *> ValueInfoSetVector;

class AnalysisResultId {
#ifndef NDEBUG
  // For building a human-readable name for this object.
  const AlgorithmId *AlgoId;
  ValueInfo *Input;
#endif

public:
  AnalysisResultId(const AlgorithmId *AlgoId, ValueInfo *Input)
#ifndef NDEBUG
    : AlgoId(AlgoId), Input(Input)
#endif
    {}

  static AnalysisResultId emptySetId() { return AnalysisResultId(0, 0); }

  std::string buildNodeLabel(const Data &Data) const;
};

class AnalysisResult : private AnalysisResultId, public GraphNode {
  friend class EnumerationContext;
  friend class Enumerator;
  friend class ScopedSetEnumerating;

  int EnumerationDepth;
  ValueInfoSetVector Set;
  AnalysisResultWorkList Work;

public:
  AnalysisResult(AnalysisResultId Id);
  virtual ~AnalysisResult();

  bool addValueInfo(ValueInfo *VI) { return Set.insert(VI); }

  // If WorkEntry is a RecursiveEnumerate whose target is "this", then it's
  // superfluous, but in practice that doesn't happen.
  void addWork(AnalysisResultWork *Entry) { Work.push_back(Entry); }

  const ValueInfoSetVector &getSetContentsSoFar() const { return Set; }

  bool isDone() const { return Work.empty(); }

  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual std::string getNodeLabel(const Data &Data) const;
  virtual bool isNodeHidden() const;

private:
  bool isEnumerating() const { return EnumerationDepth >= 0; }
};

}
}

#endif
