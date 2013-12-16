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

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenGraphNode.h"
#include "LazyAndersenValueInfo.h"

#include <vector>

namespace llvm {
class Function;
class Value;

namespace andersen_internal {

// TODO: Should this be a ValueMap?
typedef DenseMap<const Value *, ValueInfo::Ref> ValueInfoMap;
typedef std::vector<ValueInfo::Ref> ValueInfoVector;

class LazyAndersenData : public GraphNode {
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
  // A special always-empty AR for use with getPointsToSet.
  AnalysisResult EmptyAnalysisResult;

  virtual ~LazyAndersenData();

  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual std::string getNodeLabel(const LazyAndersenData &Data) const;
  virtual bool isNodeHidden() const;

private:
  LazyAndersenData(ValueInfo *ExternallyLinkableRegions,
                   ValueInfo *ExternallyAccessibleRegions);
};

}
}

#endif
