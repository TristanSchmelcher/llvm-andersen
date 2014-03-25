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
#include "Constraints.h"
#include "GraphNode.h"
#include "SubsetWork.h"
#include "TransformWork.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"

#include <utility>
#include <vector>

namespace llvm {

class raw_ostream;

}

namespace llvm {
namespace andersen_internal {

class DebugInfo;
class EnumerationResult;
class ValueInfo;
typedef SmallVector<Constraints, 1> ConstraintsVector;
typedef DenseMap<ValueInfo *, ConstraintsVector> ContentsMap;

class AnalysisResult : public GraphNode {
  friend class EnumerationContext;
  friend class ScopedSetEnumerating;

  int EnumerationDepth;
  AnalysisResultWorkList Work;
  ContentsMap Contents;

  typedef std::vector<std::pair<ValueInfo *, Constraints> >
      EnumerationHistoryVector;
  EnumerationHistoryVector EnumerationHistory;

  typedef DenseSet<AnalysisResult *> AnalysisResultSet;
  AnalysisResultSet Subsets;

public:
  AnalysisResult();
  virtual ~AnalysisResult();

  bool addContent(ValueInfo *VI, const Constraints &C);
  // Prepare for possibly adding "Subset" to the work list as a
  // SubsetWork. Returns true if it should be added, else false.
  bool prepareForSubset(AnalysisResult *Subset);
  EnumerationResult enumerate(int Depth, int LastTransformDepth,
      Constraints *C, size_t &i);
  void writeEquation(const DebugInfo &DI, raw_ostream &OS) const;

  // INSTRUCTION_ANALYSIS_PHASE only.
  void appendSubset(AnalysisResult *Entry) {
    if (prepareForSubset(Entry)) {
      Work.push_back(new SubsetWork(Entry));
    }
  }

  // INSTRUCTION_ANALYSIS_PHASE only.
  void appendUniqueTransform(TransformWork *Entry) {
    Work.push_back(Entry);
  }

  const ContentsMap &getContentsSoFar() const { return Contents; }

  size_t getNumValuesEnumeratedSoFar() const {
    return EnumerationHistory.size();
  }

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
