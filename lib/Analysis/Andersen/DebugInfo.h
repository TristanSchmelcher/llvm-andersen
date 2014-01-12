//===- DebugInfo.h - global information for debugging output --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for global debugging information.
//
//===----------------------------------------------------------------------===//

#ifndef DEBUGINFO_H
#define DEBUGINFO_H

#include "llvm/ADT/DenseMap.h"

#include <cassert>
#include <string>
#include <utility>

namespace llvm {

class raw_ostream;
class Value;

}

namespace llvm {
namespace andersen_internal {

class AlgorithmId;
class AnalysisResult;
class Data;
class ValueInfo;

class DebugInfoFiller {
protected:
  typedef std::pair<const ValueInfo *, const AlgorithmId *>
      AnalysisResultInfo;
  typedef DenseMap<const AnalysisResult *, AnalysisResultInfo>
      AnalysisResultInfoMap;
  AnalysisResultInfoMap ARIM;

public:
  void fill(const AnalysisResult *AR, const ValueInfo *VI,
            const AlgorithmId *Id) {
    bool inserted = ARIM.insert(
        std::make_pair(AR, AnalysisResultInfo(VI, Id))).second;
    (void)inserted;
    assert(inserted);
  }
};

class DebugInfo : private DebugInfoFiller {
  const Data *D;

public:
  DebugInfo(const Data *D);

  const Data *getData() const { return D; }

  static void printValueName(const Value *V, raw_ostream &OS);

  void printValueInfoName(const ValueInfo *VI, raw_ostream &OS) const;

  void printAnalysisResultName(const AnalysisResult *AR, raw_ostream &OS) const;
};

}
}

#endif
