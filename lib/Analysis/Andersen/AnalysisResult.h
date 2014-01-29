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
#include "RecursiveEnumerate.h"
#include "TransformStepBase.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SetVector.h"

namespace llvm {

class raw_ostream;

}

namespace llvm {
namespace andersen_internal {

class DebugInfo;
class EnumerationResult;
class ValueInfo;
typedef SetVector<ValueInfo *> ValueInfoSetVector;

class AnalysisResult : public GraphNode {
  friend class EnumerationContext;
  friend class ScopedSetEnumerating;

  int EnumerationDepth;
  ValueInfoSetVector Set;
  AnalysisResultWorkList Work;
  typedef DenseSet<AnalysisResult *> AnalysisResultSet;
  AnalysisResultSet Subsets;

public:
  AnalysisResult();
  virtual ~AnalysisResult();

  bool addValueInfo(ValueInfo *VI);
  // Prepare for possibly adding "Subset" to the work list as a
  // RecursiveEnumerate. Returns true if it should be added, else false.
  bool prepareForSubset(AnalysisResult *Subset);
  EnumerationResult enumerate(int Depth, int LastTransformDepth, size_t &i);
  void writeEquation(const DebugInfo &DI, raw_ostream &OS) const;

  // INSTRUCTION_ANALYSIS_PHASE only.
  void appendSubset(AnalysisResult *Entry) {
    if (prepareForSubset(Entry)) {
      Work.push_back(new RecursiveEnumerate(Entry));
    }
  }

  // INSTRUCTION_ANALYSIS_PHASE only.
  void appendUniqueTransform(TransformStepBase *Entry) {
    Work.push_back(Entry);
  }

  const ValueInfoSetVector &getSetContentsSoFar() const { return Set; }

  bool isDone() const { return Work.empty(); }

  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;
  virtual bool isNodeHidden() const;

private:
  bool isEnumerating() const { return EnumerationDepth >= 0; }
};

}
}

#endif
