//===- LazyAndersenValueInfo.cpp - value information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type holding all information for a given Value.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenValueInfo-inl.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenHalfRelationList-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

ValueInfo *const ValueInfo::Nil = 0;

ValueInfo::ValueInfo(const Value *V, Map *ContainingMap)
  : V(V), ContainingMap(ContainingMap) {}

ValueInfo::~ValueInfo() {}
