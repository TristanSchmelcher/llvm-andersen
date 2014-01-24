//===- llvm/Analysis/AndersenEnumerator.h - enumerator for AndersenPass ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares an enumerator for the contents of lazy sets created by
// AndersenPass.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ANALYSIS_ANDERSENENUMERATOR_H
#define LLVM_ANALYSIS_ANDERSENENUMERATOR_H

#include <cstddef>

namespace llvm {
namespace andersen_internal {

class EnumerationState;
class ValueInfo;

}
}

namespace llvm {

class AndersenEnumerator {
  andersen_internal::EnumerationState *const ES;
  size_t i;

public:
  explicit AndersenEnumerator(
      andersen_internal::EnumerationState *ES, size_t i = 0) : ES(ES), i(i) {}

  // Get next VI or null if done.
  andersen_internal::ValueInfo *enumerate();
};

}

#endif
