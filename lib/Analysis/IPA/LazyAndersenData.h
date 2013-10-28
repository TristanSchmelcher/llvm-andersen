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
namespace lazyandersen {

// TODO: Should this be a ValueMap?
typedef DenseMap<const Value *, ValueInfo::Ref> ValueInfoMap;

class LazyAndersenData : public GraphNode {
public:
  ValueInfoMap ValueInfos;

  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual std::string getNodeLabel() const;
  virtual bool isNodeHidden() const;

  virtual ~LazyAndersenData() {}
};

}
}

#endif
