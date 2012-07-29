//===- LazyAndersenAnalysisStep.h - analysis classes ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the class for one step of a lazy analysis algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISSTEP_H
#define LAZYANDERSENANALYSISSTEP_H

#include "LazyAndersenIntrusiveListNode.h"
#include "LazyAndersenIntrusiveListTraits.h"

#include <utility>

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  class AnalysisStep :
      protected IntrusiveListNode<AnalysisStep> {
    friend struct ilist_nextprev_traits<AnalysisStep>;
    friend struct ilist_node_traits<AnalysisStep>;
    friend struct IntrusiveListTraits<AnalysisStep>;
    friend struct ilist_traits<AnalysisStep>;

  public:
    // 2-tuple of next ValueInfo result and next AnalysisStep to run
    // to produce the following results. One or both may be 0.
    typedef std::pair<ValueInfo *, AnalysisStep *> Result;

    AnalysisStep();
    virtual ~AnalysisStep();
    virtual Result run() = 0;

  protected:
    // Get the step after this in the list, or 0 if none.
    AnalysisStep *getNextStep();
  };
}
}

namespace llvm {
  INTRUSIVE_LIST_TRAITS(lazyandersen::AnalysisStep)
}

#endif
