//===- LazyAndersenMetaAnalysisStep.cpp - analysis classes ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for meta analysis--i.e., analysis of other analysis
// results.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenMetaAnalysisStep.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenSavedIterator-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

MetaAnalysisStep::MetaAnalysisStep(
    AnalysisResult *AR)
  : Iterator(AR, AR->begin()) {}

MetaAnalysisStep::~MetaAnalysisStep() {}

void MetaAnalysisStep::run() {
  {
    ScopedLoadStoreIterator<AnalysisResultEntry> i(&Iterator);
    while (i != Iterator.getList()->end()) {
      // TODO
    }
  }
  // No entries left to analyze.
  done();
}
