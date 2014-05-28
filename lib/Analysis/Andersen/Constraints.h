//===- Constraints.h ------------------------------------------------------===//
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

#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H

#include "llvm/ADT/SmallVector.h"

namespace llvm {

class Instruction;

}

namespace llvm {
namespace andersen_internal {

struct Constraints {
  // Constraint on the call stack context. The elements are call or invoke
  // instructions in order of invocation. (i.e., caller must be the last
  // element, its caller must be the second-last element, etc.)
  SmallVector<const Instruction *, 1> CallStackContext;

  // Determine whether the satisfaction of "this" implies the satisfaction of
  // "that".
  bool implies(const Constraints &that) const;

  // Determines if it is possible for two constraints to be satisfied by the
  // same context.
  static bool areMutuallySatisfiable(const Constraints &C1,
                                     const Constraints &C2) {
    // Constraints are hierarchical, so if neither implies the other then they
    // are not mutually satisfiable.
    return C1.implies(C2) || C2.implies(C1);
  }
};

}
}

#endif
