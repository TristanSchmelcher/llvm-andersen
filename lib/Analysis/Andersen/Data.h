//===- Data.h - all analysis data -----------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type holding all analysis information.
//
//===----------------------------------------------------------------------===//

#ifndef DATA_H
#define DATA_H

#include "AnalysisResult.h"
#include "GraphNode.h"
#include "ValueInfo.h"
#include "llvm/ADT/DenseMap.h"

#include <vector>

namespace llvm {

class Function;
class Instruction;
class raw_ostream;
class Value;

}

namespace llvm {
namespace andersen_internal {

class DebugInfo;
class DebugInfoFiller;

// TODO: Should this be a ValueMap?
typedef DenseMap<const Value *, ValueInfo::Ref> ValueInfoMap;
typedef std::vector<ValueInfo::Ref> ValueInfoVector;

class Data : public GraphNode {
  friend class InstructionAnalyzer;

public:
  // ValueInfo for all Values used in the Module.
  ValueInfoMap ValueInfos;
  // ValueInfo for all global regions (GlobalVariable or Function) defined in
  // the Module. These only differ from the ValueInfos entry if the symbol is
  // overridable.
  ValueInfoMap GlobalRegionInfos;
  // The special VI for regions that can be linked into other translation units.
  const ValueInfo::Ref ExternallyLinkableRegions;
  // The special VI for all regions that can be accessed by other translation
  // units.
  const ValueInfo::Ref ExternallyAccessibleRegions;
  // VIs not associated with any Value (e.g., generated for intrinsics).
  ValueInfoVector AnonymousValueInfos;
  // Interned dynamically-created algorithm ids.
  DenseMap<Instruction *, AlgorithmId *> PushCallSiteAlgorithmIds;
  DenseMap<Instruction *, AlgorithmId *> PopCallSiteAlgorithmIds;

  virtual ~Data();

  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;
  virtual bool isNodeHidden() const;

  void fillDebugInfo(DebugInfoFiller *DIF) const;
  void writeEquations(const DebugInfo &DI, raw_ostream &OS) const;

private:
  Data(ValueInfo *ExternallyLinkableRegions,
       ValueInfo *ExternallyAccessibleRegions);

  typedef void (*ValueInfoVisitorFn)(void *, ValueInfo *);

  void visitValueInfos(ValueInfoVisitorFn visitor, void *Arg) const;
};

}
}

#endif
