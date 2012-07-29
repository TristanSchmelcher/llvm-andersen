//===- LazyAndersenHalfRelationBase.cpp - relations classes ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the base type for one direction of a relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenHalfRelationBase.h"

#include "LazyAndersenHalfRelationBaseList.h"

#include <cassert>

using namespace llvm;
using namespace llvm::lazyandersen;

HalfRelationBase::HalfRelationBase(
    HalfRelationBaseList *InitialList) {
  assert(InitialList);
  InitialList->push_back(this);
  assert(getList() == InitialList);
}

HalfRelationBase::~HalfRelationBase() {
  if (getList()) {
    getList()->remove(HalfRelationBaseList::iterator(this));
    assert(!getList());
  }
}
