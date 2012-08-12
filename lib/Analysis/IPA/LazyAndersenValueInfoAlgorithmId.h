//===- LazyAndersenValueInfoAlgorithmId.h - algorithm classes -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares ids for ValueInfo algorithms.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENVALUEINFOALGORITHMID_H
#define LAZYANDERSENVALUEINFOALGORITHMID_H

#include "LazyAndersenAlgorithmGroup.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;
  class AnalysisResult;

  enum ValueInfoAlgorithmId {
    POINTS_TO_SET,
    REVERSE_POINTS_TO_SET,
    // TODO
    NUM_VALUE_INFO_ALGORITHM_IDS
  };

  CREATE_ALGORITHM_GROUP(ValueInfoAlgorithmId, NUM_VALUE_INFO_ALGORITHM_IDS,
      ValueInfo, AnalysisResult);
}
}

#endif
