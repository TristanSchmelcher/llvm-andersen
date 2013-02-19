//===- LazyAndersenRelation.cpp - relation classes ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a base type for relations--i.e., directed edges in the
// analysis graph.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenRelation.h"

using namespace llvm;
using namespace llvm::lazyandersen;

Relation::Relation(ValueInfo *Src, ValueInfo *Dst)
  : HalfRelation<INCOMING>(Src), HalfRelation<OUTGOING>(Dst) {}

Relation::~Relation() {}
