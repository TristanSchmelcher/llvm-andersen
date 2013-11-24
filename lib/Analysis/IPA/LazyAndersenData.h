//===- LazyAndersenData.h - all analysis data -----------------------------===//
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

#ifndef LAZYANDERSENDATA_H
#define LAZYANDERSENDATA_H

#include "LazyAndersenGraphNode.h"
#include "LazyAndersenValueInfo.h"

namespace llvm {
class Function;
class Value;

namespace lazyandersen {

// TODO: Should this be a ValueMap?
typedef DenseMap<const Value *, ValueInfo::Ref> ValueInfoMap;
typedef DenseMap<const Function *, ValueInfo::Ref> FunctionMap;

class LazyAndersenData : public GraphNode {
  friend class InstructionAnalyzer;

public:
  // ValueInfo for all Values used in the Module.
  ValueInfoMap ValueInfos;
  // ValueInfo for all Functions defined in the Module. These only differ from
  // the ValueInfos entry if the function is overridable. 
  FunctionMap FunctionInfos;
  const ValueInfo::Ref ExternallyDefinedRegions;
  const ValueInfo::Ref ExternallyLinkableRegions;
  const ValueInfo::Ref ExternallyAccessibleRegions;

  virtual ~LazyAndersenData();

  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual std::string getNodeLabel(const LazyAndersenData &Data) const;
  virtual bool isNodeHidden() const;

private:
  LazyAndersenData(ValueInfo *ExternallyDefinedRegions,
                   ValueInfo *ExternallyLinkableRegions,
                   ValueInfo *ExternallyAccessibleRegions);
};

}
}

#endif
