//===- LazyAndersenInstructionAnalysisAlgorithm.cpp -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a base type for algorithms that operate exclusively by
// hooking into the instruction analyzer.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenInstructionAnalysisAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"

using namespace llvm;
using namespace llvm::andersen_internal;

AnalysisResult *InstructionAnalysisAlgorithm::run(ValueInfo *VI) {
  return new AnalysisResult();
}
