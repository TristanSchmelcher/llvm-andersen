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

namespace llvm {
namespace andersen_internal {

namespace {

struct WriteEquationsArg {
  WriteEquationsArg(const Data *D, raw_ostream *OS) : D(D), OS(OS) {}

  const Data *D;
  raw_ostream *OS;
};


void writeEquationsVisitor(void *Arg, ValueInfo *VI) {
  WriteEquationsArg *WEA = static_cast<WriteEquationsArg *>(Arg);
  VI->writeEquations(*WEA->D, *WEA->OS);
}

void getOutgoingEdgesVisitor(void *Arg, ValueInfo *VI) {
  GraphEdgeDeque *Result = static_cast<GraphEdgeDeque *>(Arg);
  // No edge label needed because the edges will not be printed.
  Result->push_back(GraphEdge(VI, std::string()));
}

}

Data::Data(ValueInfo *ExternallyLinkableRegions,
           ValueInfo *ExternallyAccessibleRegions)
  : ExternallyLinkableRegions(ExternallyLinkableRegions),
    ExternallyAccessibleRegions(ExternallyAccessibleRegions),
    EmptyAnalysisResult(AnalysisResultId::emptySetId()) {}

Data::~Data() {}

GraphEdgeDeque Data::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  visitValueInfos(&getOutgoingEdgesVisitor, static_cast<void *>(&Result));
  return Result;
}

std::string Data::getNodeLabel(const Data &Data) const {
  llvm_unreachable("Data node should not be emitted in the graph");
  return std::string();
}

bool Data::isNodeHidden() const {
  return true;
}

void Data::writeEquations(raw_ostream &OS) const {
  WriteEquationsArg WEA(this, &OS);
  visitValueInfos(&writeEquationsVisitor, static_cast<void *>(&WEA));
}

void Data::visitValueInfos(ValueInfoVisitorFn visitor, void *Arg) const {
  for (ValueInfoMap::const_iterator i = ValueInfos.begin(),
                                    End = ValueInfos.end();
       i != End; ++i) {
    if (!i->second.getPtr() || i->first != i->second->getValue()) continue;
    (*visitor)(Arg, i->second.getPtr());
  }
  for (ValueInfoMap::const_iterator i = GlobalRegionInfos.begin(),
                                    End = GlobalRegionInfos.end();
       i != End; ++i) {
    (*visitor)(Arg, i->second.getPtr());
  }
  (*visitor)(Arg, ExternallyLinkableRegions.getPtr());
  (*visitor)(Arg, ExternallyAccessibleRegions.getPtr());
  for (ValueInfoVector::const_iterator i = AnonymousValueInfos.begin(),
                                       End = AnonymousValueInfos.end();
       i != End; ++i) {
    (*visitor)(Arg, i->getPtr());
  }
}

}
}
