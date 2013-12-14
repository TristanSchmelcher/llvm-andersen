//===- LazyAndersen.cpp - Lazy Andersen's algorithm -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the LazyAndersen pass, which is a subset-based and
// context-, flow-, and field-insensitive points-to analysis based on a lazy
// version of Andersen's algorithm.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "lazy-andersen"
#include "llvm/Analysis/LazyAndersen.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenData.h"
#include "LazyAndersenInstructionAnalyzer.h"
#include "LazyAndersenPhase.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenTopEnumerator.h"
#include "LazyAndersenValuePrinter.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {

cl::opt<bool> NonLazy("andersen-non-lazy",
                      cl::desc("Perform Andersen analysis non-lazily"));

}

char LazyAndersen::ID = 0;
// TODO: What do these two bools mean?
INITIALIZE_PASS_BEGIN(LazyAndersen, "lazy-andersen",
                      "Lazy Andersen's Algorithm for Points-To Analysis", false,
                      true)
INITIALIZE_PASS_DEPENDENCY(DominatorTree)
INITIALIZE_PASS_END(LazyAndersen, "lazy-andersen",
                    "Lazy Andersen's Algorithm for Points-To Analysis", false,
                    true)

namespace {

std::string prettyPrintValueOrExternal(const Value *V) {
  if (V) {
    return prettyPrintValue(V);
  } else {
    // Placeholder for externally-defined regions.
    return "EXTERNAL";
  }
}

}

LazyAndersen::LazyAndersen()
  : ModulePass(ID), Data(0) {
  initializeLazyAndersenPass(*PassRegistry::getPassRegistry());
}

const SetVector<ValueInfo *> *LazyAndersen::getPointsToSet(const Value *V)
    const {
  AnalysisResult *AR = getPointsToSetAnalysisResult(V);
  if (!AR) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  // Else it could point to something. Finish any deferred work.
  if (!AR->isDone()) {
    for (TopEnumerator TE(AR, AR->getSetContentsSoFar().size());
         TE.enumerate(); );
    assert(AR->isDone());
  }
  if (AR->getSetContentsSoFar().empty()) {
    // Doesn't point to anything after all. Return null for consistency.
    return 0;
  }
  return &AR->getSetContentsSoFar();
}

TopEnumerator LazyAndersen::enumeratePointsToSet(const Value *V) const {
  AnalysisResult *AR = getPointsToSetAnalysisResult(V);
  if (!AR) {
    // We determined this points to nothing at instruction analysis time.
    return TopEnumerator(&Data->EmptyAnalysisResult);
  }
  return TopEnumerator(AR);
}

AnalysisResult *LazyAndersen::getPointsToSetAnalysisResult(const Value *V)
    const {
  ValueInfoMap::const_iterator i = Data->ValueInfos.find(V);
  if (!V) {
    // This can only happen if we are being queried for an unreachable
    // instruction. Pretend its result points to nothing.
    // TODO: Write an assert that verifies this.
    return 0;
  }
  ValueInfo *VI = i->second.getPtr();
  if (!VI) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  return VI->getAlgorithmResult<PointsToAlgorithm, ENUMERATION_PHASE>();
}

bool LazyAndersen::runOnModule(Module &M) {
  assert(!Data);
  Data = InstructionAnalyzer::run(this, M);
  if (NonLazy) {
    for (ValueInfoMap::const_iterator i = Data->ValueInfos.begin();
         i != Data->ValueInfos.end(); ++i) {
      getPointsToSet(i->first);
    }
  }
  return false;
}

void LazyAndersen::releaseMemory() {
  delete Data;
  Data = 0;
}

void LazyAndersen::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  AU.addRequired<DominatorTree>();
}

void LazyAndersen::print(raw_ostream &OS, const Module *M) const {
  assert(Data);
  if (!NonLazy) {
    OS << "Nothing to print. Re-run with -andersen-non-lazy to see points-to"
          " analysis results.\n";
    return;
  }
  for (ValueInfoMap::const_iterator i = Data->ValueInfos.begin();
       i != Data->ValueInfos.end(); ++i) {
    const ValueInfoSetVector *PointsToSet = getPointsToSet(i->first);
    OS << "Points-to set for " << prettyPrintValue(i->first) << ":\n";
    if (PointsToSet) {
      for (ValueInfoSetVector::const_iterator i = PointsToSet->begin();
           i != PointsToSet->end(); ++i) {
        OS << "  " << prettyPrintValueOrExternal((*i)->getValue()) << "\n";
      }
    }
  }
}
