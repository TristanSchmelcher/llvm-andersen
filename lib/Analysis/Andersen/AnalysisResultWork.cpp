//===- AnalysisResultWork.cpp - analysis classes --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for pending work for analysis results.
//
//===----------------------------------------------------------------------===//

#include "AnalysisResultWork.h"

namespace llvm {
namespace andersen_internal {

AnalysisResultWork::AnalysisResultWork() {}

AnalysisResultWork::~AnalysisResultWork() {}

bool AnalysisResultWork::isNodeHidden() const {
  return false;
}

}
}
