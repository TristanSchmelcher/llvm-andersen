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

#include "AlgorithmId.h"
#include "DebugInfo.h"
#include "PushPopCallSiteAlgorithmId.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>

namespace llvm {
namespace andersen_internal {

namespace {

void getOutgoingEdgesVisitor(void *Arg, ValueInfo *VI) {
  GraphEdgeDeque *Result = static_cast<GraphEdgeDeque *>(Arg);
  // No edge label needed because the edges will not be printed.
  Result->push_back(GraphEdge(VI, std::string()));
}

struct WriteEquationsArg {
  WriteEquationsArg(const DebugInfo *DI, raw_ostream *OS) : DI(DI), OS(OS) {}

  const DebugInfo *DI;
  raw_ostream *OS;
};

void fillDebugInfoVisitor(void *Arg, ValueInfo *VI) {
  DebugInfoFiller *DIF = static_cast<DebugInfoFiller *>(Arg);
  VI->fillDebugInfo(DIF);
}

void writeEquationsVisitor(void *Arg, ValueInfo *VI) {
  WriteEquationsArg *WEA = static_cast<WriteEquationsArg *>(Arg);
  VI->writeEquations(*WEA->DI, *WEA->OS);
}

}

Data::Data(ValueInfo *ExternallyLinkableRegions,
           ValueInfo *ExternallyAccessibleRegions)
  : ExternallyLinkableRegions(ExternallyLinkableRegions),
    ExternallyAccessibleRegions(ExternallyAccessibleRegions) {}

Data::~Data() {
  DeleteContainerSeconds(PushPopCallSiteAlgorithmIds);
}

GraphEdgeDeque Data::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  visitValueInfos(&getOutgoingEdgesVisitor, static_cast<void *>(&Result));
  return Result;
}

void Data::printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const {
  llvm_unreachable("Data node should not be emitted in the graph");
}

bool Data::isNodeHidden() const {
  return true;
}

void Data::fillDebugInfo(DebugInfoFiller *DIF) const {
  visitValueInfos(&fillDebugInfoVisitor, static_cast<void *>(DIF));
}

void Data::writeEquations(const DebugInfo &DI, raw_ostream &OS) const {
  {
    WriteEquationsArg WEA(&DI, &OS);
    visitValueInfos(&writeEquationsVisitor, static_cast<void *>(&WEA));
  }
  // Also print all the equivalences.
  for (ValueInfoMap::const_iterator i = ValueInfos.begin(),
                                    End = ValueInfos.end();
       i != End; ++i) {
    if (!i->second.getPtr()) {
      OS << "nil";
    } else {
      if (i->first == i->second->getValue()) continue;  // Already handled.
      DI.printValueInfoName(i->second.getPtr(), OS);
    }
    OS << " <=> ";
    DebugInfo::printValueName(i->first, OS);
    OS << '\n';
  }
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
    ValueInfoMap::const_iterator j = ValueInfos.find(i->first);
    assert(j != ValueInfos.end());
    if (i->second.getPtr() != j->second.getPtr()) {
      (*visitor)(Arg, i->second.getPtr());
    }
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
