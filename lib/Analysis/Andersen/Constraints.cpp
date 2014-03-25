//===- Constraints.cpp - algorithm classes --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// A Constraints represents a logical AND of constraints that must be satisfied
// by the context for a corresponding value to appear in a set.
//
//===----------------------------------------------------------------------===//

#include "Constraints.h"

#include <algorithm>

namespace llvm {
namespace andersen_internal {

bool Constraints::implies(const Constraints &that) const {
  if (!(this->CallStackContext.size() >= that.CallStackContext.size() &&
        std::equal(that.CallStackContext.begin(),
                   that.CallStackContext.end(),
                   this->CallStackContext.end() -
                       that.CallStackContext.size()))) {
    return false;
  }

  return true;
}

}
}
