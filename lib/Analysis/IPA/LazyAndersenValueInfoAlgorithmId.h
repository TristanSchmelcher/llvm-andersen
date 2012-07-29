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

#include "LazyAndersenAlgorithmTraits.h"

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  enum ValueInfoAlgorithmId {
    POINTS_TO_SET,
    REVERSE_POINTS_TO_SET,
    NUM_VALUE_INFO_ALGORITHM_IDS
  };

  ALGORITHM_GROUP_TRAITS(ValueInfoAlgorithmId,
                         NUM_VALUE_INFO_ALGORITHM_IDS,
                         ValueInfo);
}
}

#endif
