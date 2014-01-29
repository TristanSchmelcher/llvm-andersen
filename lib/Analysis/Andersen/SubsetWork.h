//===- SubsetWork.h - algorithm classes -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for enumerating the elements of an AnalysisResult
// that come from a certain subset of it.
//
//===----------------------------------------------------------------------===//

#ifndef SUBSETWORK_H
#define SUBSETWORK_H

#include "AnalysisResultWork.h"
#include "Enumerator.h"

namespace llvm {
namespace andersen_internal {

class AnalysisResult;

class SubsetWork : public AnalysisResultWork {
  Enumerator E;

public:
  SubsetWork(AnalysisResult *AR);
  ~SubsetWork();
  virtual EnumerationResult enumerate(EnumerationContext *Ctx);
  virtual bool prepareForRewrite(AnalysisResult *RewriteTarget) const;
  virtual void writeFormula(const DebugInfo &DI, raw_ostream &OS) const;
  virtual GraphEdgeDeque getOutgoingEdges() const;
  virtual void printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const;
};

}
}

#endif
