//===- LazyAndersenReversePointsToAlgorithm.cpp - reverse points-to -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the reverse points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenReversePointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenArgumentReversePointsToAlgorithm.h"
#include "LazyAndersenContentReversePointsToAlgorithm.h"
#include "LazyAndersenRecursiveEnumerate.h"
#include "LazyAndersenReturnValueReversePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char ReversePointsToAlgorithm::ID[] = "reverse points-to";

void ReversePointsToAlgorithm::RelationHandler<ARGUMENT_TO_CALLEE>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->getOrCreateEagerAlgorithmResult<
      ReversePointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Dst->getAlgorithmResult<
                  ArgumentReversePointsToAlgorithm>()));
}

void ReversePointsToAlgorithm::RelationHandler<DEPENDS_ON>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Dst->getOrCreateEagerAlgorithmResult<
      ReversePointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Src->getOrCreateEagerAlgorithmResult<
                  ReversePointsToAlgorithm>()));
}

void ReversePointsToAlgorithm::RelationHandler<RETURNED_TO_CALLER>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->getOrCreateEagerAlgorithmResult<
      ReversePointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Dst->getAlgorithmResult<
                  ReturnValueReversePointsToAlgorithm>()));
}

void ReversePointsToAlgorithm::RelationHandler<STORED_TO>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->getOrCreateEagerAlgorithmResult<
      ReversePointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Dst->getAlgorithmResult<
                  ContentReversePointsToAlgorithm>()));
}
