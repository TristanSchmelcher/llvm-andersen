//===- AndersenPass.cpp - Andersen's algorithm ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// The AndersenPass class is an LLVM pass which implements Andersen's algorithm
// for points-to analysis with some modifications for lazy evaluation.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "andersen"
#include "llvm/Analysis/AndersenPass.h"

#include "AnalysisResult.h"
#include "Data.h"
#include "InstructionAnalyzer.h"
#include "Phase.h"
#include "PointsToAlgorithm.h"
#include "ValuePrinter.h"
#include "llvm/Analysis/AndersenEnumerator.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

namespace llvm {

using namespace andersen_internal;

namespace {

cl::opt<bool> NonLazy("andersen-non-lazy",
                      cl::desc("Perform Andersen analysis non-lazily"));

std::string prettyPrintValueOrExternal(const Value *V) {
  if (V) {
    return ValuePrinter::prettyPrintValue(V);
  } else {
    // Placeholder for externally-defined regions.
    return "EXTERNAL";
  }
}

}

char AndersenPass::ID = 0;

AndersenPass::AndersenPass()
  : ModulePass(ID), Data(0) {
  initializeAndersenPassPass(*PassRegistry::getPassRegistry());
}

const PointsToSet *AndersenPass::getPointsToSet(const Value *V) const {
  AnalysisResult *AR = getPointsToSetAnalysisResult(V);
  if (!AR) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  // Else it could point to something. Finish any deferred work.
  if (!AR->isDone()) {
    for (AndersenEnumerator AE(AR, AR->getSetContentsSoFar().size());
         AE.enumerate(); );
    assert(AR->isDone());
  }
  if (AR->getSetContentsSoFar().empty()) {
    // Doesn't point to anything after all. Return null for consistency.
    return 0;
  }
  return &AR->getSetContentsSoFar();
}

AndersenEnumerator AndersenPass::enumeratePointsToSet(const Value *V) const {
  AnalysisResult *AR = getPointsToSetAnalysisResult(V);
  if (!AR) {
    // We determined this points to nothing at instruction analysis time.
    return AndersenEnumerator(&Data->EmptyAnalysisResult);
  }
  return AndersenEnumerator(AR);
}

AnalysisResult *AndersenPass::getPointsToSetAnalysisResult(const Value *V)
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

bool AndersenPass::runOnModule(Module &M) {
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

void AndersenPass::releaseMemory() {
  delete Data;
  Data = 0;
}

void AndersenPass::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  AU.addRequired<DominatorTree>();
}

void AndersenPass::print(raw_ostream &OS, const Module *M) const {
  assert(Data);
  if (!NonLazy) {
    OS << "Nothing to print. Re-run with -andersen-non-lazy to see points-to"
          " analysis results.\n";
    return;
  }
  for (ValueInfoMap::const_iterator i = Data->ValueInfos.begin();
       i != Data->ValueInfos.end(); ++i) {
    const ValueInfoSetVector *PointsToSet = getPointsToSet(i->first);
    OS << "Points-to set for " << ValuePrinter::prettyPrintValue(i->first)
       << ":\n";
    if (PointsToSet) {
      for (ValueInfoSetVector::const_iterator i = PointsToSet->begin();
           i != PointsToSet->end(); ++i) {
        OS << "  " << prettyPrintValueOrExternal((*i)->getValue()) << "\n";
      }
    }
  }
}

}

using namespace llvm;

// TODO: What do these two bools mean?
INITIALIZE_PASS_BEGIN(AndersenPass, "andersen",
                      "Andersen's Algorithm for Points-To Analysis", false,
                      true)
INITIALIZE_PASS_DEPENDENCY(DominatorTree)
INITIALIZE_PASS_END(AndersenPass, "andersen",
                    "Andersen's Algorithm for Points-To Analysis", false,
                    true)
