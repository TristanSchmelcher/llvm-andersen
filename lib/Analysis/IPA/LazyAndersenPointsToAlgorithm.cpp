//===- LazyAndersenPointsToAlgorithm.cpp - points-to algorithm ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for the main points-to algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenPointsToAlgorithm.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenArgumentPointsToAlgorithm.h"
#include "LazyAndersenContentPointsToAlgorithm.h"
#include "LazyAndersenRecursiveEnumerate.h"
#include "LazyAndersenReturnValuePointsToAlgorithm.h"
#include "LazyAndersenValueInfo.h"

using namespace llvm;
using namespace llvm::lazyandersen;

const char PointsToAlgorithm::ID[] = "points-to";

void PointsToAlgorithm::RelationHandler<ARGUMENT_FROM_CALLER>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->getOrCreateEagerAlgorithmResult<
      PointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Dst->getAlgorithmResult<
                  ArgumentPointsToAlgorithm>()));
}

void PointsToAlgorithm::RelationHandler<DEPENDS_ON>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->getOrCreateEagerAlgorithmResult<
      PointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Dst->getOrCreateEagerAlgorithmResult<
                  PointsToAlgorithm>()));
}

void PointsToAlgorithm::RelationHandler<LOADED_FROM>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->getOrCreateEagerAlgorithmResult<
      PointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Dst->getAlgorithmResult<
                  ContentPointsToAlgorithm>()));
}

void PointsToAlgorithm::RelationHandler<RETURNED_FROM_CALLEE>::onRelation(
    ValueInfo *Src, ValueInfo *Dst) {
  Src->getOrCreateEagerAlgorithmResult<
      PointsToAlgorithm>()
          ->addWork(new RecursiveEnumerate(
              Dst->getAlgorithmResult<
                  ReturnValuePointsToAlgorithm>()));
}
