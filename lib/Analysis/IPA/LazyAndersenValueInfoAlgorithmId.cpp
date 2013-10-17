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
  "CONTENT_POINTS",
  "CONTENT_REVERSE_POINTS",
  "ARGUMENT_POINTS",
  "ARGUMENT_REVERSE_POINTS",
  "RETURN_VALUE_POINTS",
  "RETURN_VALUE_REVERSE_POINTS",
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
