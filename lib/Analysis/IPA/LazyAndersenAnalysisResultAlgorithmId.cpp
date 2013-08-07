//===- LazyAndersenAnalysisResultAlgorithmId.cpp - algorithm classes ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines ids for AnalysisResult algorithms.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenAnalysisResultAlgorithmId.h"

namespace llvm {
namespace lazyandersen {

const char *const AnalysisResultAlgorithmNames[
    NUM_ANALYSIS_RESULT_ALGORITHMS] = {
  "CONTENT",
  "CONTENT2",
  "CONTENT_REVERSE",
  "CONTENT_REVERSE2",
  "RETURN_VALUE",
  "RETURN_VALUE_REVERSE",
  "ARGUMENT",
  "ARGUMENT_REVERSE",
};

}
}
