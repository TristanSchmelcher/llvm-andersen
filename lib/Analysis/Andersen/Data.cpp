//===- Data.cpp - all analysis data ---------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type holding all analysis information.
//
//===----------------------------------------------------------------------===//

#include "Data.h"

#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::andersen_internal;

Data::Data(ValueInfo *ExternallyLinkableRegions,
           ValueInfo *ExternallyAccessibleRegions)
  : ExternallyLinkableRegions(ExternallyLinkableRegions),
    ExternallyAccessibleRegions(ExternallyAccessibleRegions) {}

Data::~Data() {}

GraphEdgeDeque Data::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  // No edge labels needed anywhere in here because the edges will not be
  // printed.
  for (ValueInfoMap::const_iterator i = ValueInfos.begin(),
                                    End = ValueInfos.end();
       i != End; ++i) {
    if (!i->second.getPtr() || i->first != i->second->getValue()) continue;
    Result.push_back(GraphEdge(i->second.getPtr(), std::string()));
  }
  for (ValueInfoMap::const_iterator i = GlobalRegionInfos.begin(),
                                    End = GlobalRegionInfos.end();
       i != End; ++i) {
    Result.push_back(GraphEdge(i->second.getPtr(), std::string()));
  }
  Result.push_back(GraphEdge(
      ExternallyLinkableRegions.getPtr(), std::string()));
  Result.push_back(GraphEdge(
      ExternallyAccessibleRegions.getPtr(), std::string()));
  return Result;
}

std::string Data::getNodeLabel(const Data &Data) const {
  llvm_unreachable("Data node should not be emitted in the graph");
  return std::string();
}

bool Data::isNodeHidden() const {
  return true;
}
