//===- LazyAndersenRelationsAnalysisStep.h - analysis classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a template type for relations analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONSANALYSISSTEP_H
#define LAZYANDERSENRELATIONSANALYSISSTEP_H

#include "LazyAndersenEdgeEndpointType.h"
#include "LazyAndersenRelationsAnalysisStepBase.h"

namespace llvm {
namespace lazyandersen {
  class Relation;
  class ValueInfo;

  template<EdgeEndpointType Endpoint>
  class RelationsAnalysisStep : public RelationsAnalysisStepBase {
  public:
    explicit RelationsAnalysisStep(ValueInfo *VI);
    virtual std::list<GraphEdge> getOutgoingEdges() const;

  protected:
    virtual AnalysisResult *analyzeRelation(Relation *R) = 0;

  private:
    virtual AnalysisResult *analyzeHalfRelation(HalfRelationBase *HR);
  };
}
}

#endif
