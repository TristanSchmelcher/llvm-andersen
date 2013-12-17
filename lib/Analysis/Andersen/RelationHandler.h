//===- RelationHandler.h --------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a class for translating the relations produced by
// instruction analysis into graph traversal steps.
//
//===----------------------------------------------------------------------===//

#ifndef RELATIONHANDLER_H
#define RELATIONHANDLER_H

#include "RelationType.h"

namespace llvm {
namespace andersen_internal {

class ValueInfo;

struct RelationHandler {
  // Creates graph traversal steps to handle the relation RT between Src and
  // Dst.
  template<RelationType RT>
  static void handleRelation(ValueInfo *Src, ValueInfo *Dst);
};

}
}

#endif
