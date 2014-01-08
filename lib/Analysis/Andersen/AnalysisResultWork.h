//===- AnalysisResultWork.h - algorithm classes ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for pending work for analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef ANALYSISRESULTWORK_H
#define ANALYSISRESULTWORK_H

#include "GraphNode.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"

namespace llvm {

class raw_ostream;

}

namespace llvm {
namespace andersen_internal {

class Data;
class EnumerationContext;
class EnumerationResult;

class AnalysisResultWork :
    public ilist_node<AnalysisResultWork>,
    public GraphNode {
public:
  AnalysisResultWork();
  virtual ~AnalysisResultWork();
  virtual bool isNodeHidden() const;

  virtual EnumerationResult enumerate(EnumerationContext *Ctx) = 0;
  virtual void writeFormula(const Data &Data, raw_ostream &OS) const = 0;
};

typedef iplist<AnalysisResultWork> AnalysisResultWorkList;

}
}

namespace llvm {

template<>
struct ilist_traits<andersen_internal::AnalysisResultWork>
  : public ilist_nextprev_traits<andersen_internal::AnalysisResultWork>,
    public ilist_ghostly_sentinel_traits<
        andersen_internal::AnalysisResultWork>,
    public ilist_node_traits<andersen_internal::AnalysisResultWork> {};

}

#endif
