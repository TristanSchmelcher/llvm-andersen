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

class raw_ostream;

}

namespace llvm {
namespace andersen_internal {

class DebugInfo;
class DebugInfoFiller;
class EnumerateContentResult;

class AnalysisResult {
public:
  virtual ~AnalysisResult() {}
  virtual EnumerateContentResult enumerateContent(size_t i, int Depth) = 0;
  virtual void fillDebugInfo(DebugInfoFiller *DIF) const = 0;
  virtual void writeEquation(const DebugInfo &DI, raw_ostream &OS) const = 0;
};

}
}

#endif
