//===- AnalysisResult.h ---------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef ANALYSISRESULT_H
#define ANALYSISRESULT_H

#include <cstddef>

namespace llvm {
namespace andersen_internal {

class EnumerateContentResult;

class AnalysisResult {
public:
  virtual ~AnalysisResult() {}
  virtual EnumerateContentResult enumerateContent(size_t i, int Depth) = 0;
};

}
}

#endif
