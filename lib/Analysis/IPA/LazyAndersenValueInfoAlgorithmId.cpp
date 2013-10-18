//===- LazyAndersenValueInfoAlgorithmId.cpp - algorithm classes -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines ids for ValueInfo algorithms.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenValueInfoAlgorithmId.h"

namespace llvm {
namespace lazyandersen {

const char *const ValueInfoAlgorithmNames[NUM_VALUE_INFO_ALGORITHMS] = {
  "POINTS",
  "REVERSE_POINTS",
  "CONTENT_POINTS3",
  "CONTENT_REVERSE_POINTS3",
  "ARGUMENT_POINTS2",
  "ARGUMENT_REVERSE_POINTS2",
  "RETURN_VALUE_POINTS2",
  "RETURN_VALUE_REVERSE_POINTS2",
  "CONTENT_POINTS",
  "CONTENT_POINTS2",
  "CONTENT_REVERSE_POINTS",
  "CONTENT_REVERSE_POINTS2",
  "RETURN_VALUE_POINTS",
  "RETURN_VALUE_REVERSE_POINTS",
  "ARGUMENT_POINTS",
  "ARGUMENT_REVERSE_POINTS",
};

}
}
