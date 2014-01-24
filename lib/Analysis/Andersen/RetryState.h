//===- RetryState.h -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#ifndef RETRYSTATE_H
#define RETRYSTATE_H

#include <set>

namespace llvm {
namespace andersen_internal {

class EnumerationState;

struct RetryEntry {
  RetryEntry(EnumerationState *ES)
    : EnumerationDepth(ES->EnumerationDepth), ES(ES), ExitCount(ExitCount) {}

  int EnumerationDepth;
  EnumerationState *ES;
  unsigned int ExitCount;
};

// Orders by decreasing depth.
struct RetryEntryComparator {
  bool operator()(const RetryEntry &E1, const RetryEntry &E2) const {
    return E1.EnumerationDepth > E2.EnumerationDepth;
  }
};

typedef std::set<RetryEntry, RetryEntryComparator> RetryState;

}
}

#endif
