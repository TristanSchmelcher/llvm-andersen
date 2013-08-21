//===- LazyAndersenAnalysisResultEntry.h - algorithm classes --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a template type for entries in analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULTENTRY_H
#define LAZYANDERSENANALYSISRESULTENTRY_H

#include "LazyAndersenAnalysisResultEntryBase.h"

namespace llvm {
namespace lazyandersen {
  template<AnalysisResultEntryBase::EntryType Ty>
  class AnalysisResultEntry : public AnalysisResultEntryBase {
  public:
    virtual EntryType getEntryType() const {
      return Ty;
    }

  protected:
    static bool classof(const AnalysisResultEntryBase *Base) {
      return Base->getEntryType() == Ty;
    }
  };
}
}

#endif
