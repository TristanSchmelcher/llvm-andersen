//===- LazyAndersenRelationHandler.h --------------------------------------===//
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

#ifndef LAZYANDERSENRELATIONHANDLER_H
#define LAZYANDERSENRELATIONHANDLER_H

#include "LazyAndersenRelationType.h"

namespace llvm {
namespace lazyandersen {
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
