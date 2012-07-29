//===- LazyAndersenAnalysisStepList.h - analysis classes ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for a list of analysis steps.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISSTEPLIST_H
#define LAZYANDERSENANALYSISSTEPLIST_H

#include "LazyAndersenAnalysisStep.h"

namespace llvm {
namespace lazyandersen {
  // Currently we do not need any extra information in these lists.
  typedef ilist<AnalysisStep> AnalysisStepList;
}
}

#endif
