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

#include "llvm/Analysis/AndersenPass.h"

#include "Data.h"
#include "DebugInfo.h"
#include "EnumerationState.h"
#include "InstructionAnalysisResult.h"
#include "InstructionAnalyzer.h"
#include "Phase.h"
#include "PointsToAlgorithm.h"
#include "llvm/Analysis/AndersenEnumerator.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>

namespace llvm {

using namespace andersen_internal;

namespace {

cl::opt<bool> NonLazy("andersen-non-lazy",
                      cl::desc("Perform Andersen analysis non-lazily"));

AndersenEnumerator enumerateRemaining(EnumerationState *ES) {
  return AndersenEnumerator(ES, ES->getElementsSoFar().size());
}

void writeEquations(const Data *Data, raw_ostream &OS) {
  DebugInfo DI(Data);
  Data->writeEquations(DI, OS);
}

}

char AndersenPass::ID = 0;

AndersenPass::AndersenPass()
  : ModulePass(ID), Data(0) {
  initializeAndersenPassPass(*PassRegistry::getPassRegistry());
}

AndersenHandle AndersenPass::getHandleToPointsToSet(const Value *V) const {
  assert(V);
  ValueInfoMap::const_iterator i = Data->ValueInfos.find(V);
  if (i == Data->ValueInfos.end()) {
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
  InstructionAnalysisResult *IAR = VI->getAlgorithmResult<PointsToAlgorithm,
      ENUMERATION_PHASE>();
  if (!IAR) {
    // Ditto.
    return 0;
  }
  return IAR->getEnumerationState();
}

const PointsToSet *AndersenPass::getPointsToSet(AndersenHandle AH) const {
  EnumerationState *ES = AH;
  if (!ES) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  // Else it could point to something. Finish any deferred work.
  for (AndersenEnumerator AE(enumerateRemaining(ES)); AE.enumerate(); );
  assert(ES->isDone());
  if (ES->getElementsSoFar().empty()) {
    // Doesn't point to anything after all. Return null for consistency.
    return 0;
  }
  return &ES->getElementsSoFar();
}

bool AndersenPass::isPointsToSetEmpty(AndersenHandle AH) const {
  EnumerationState *ES = AH;
  return AndersenEnumerator(ES).enumerate() != 0;
}

AndersenEnumerator AndersenPass::enumeratePointsToSet(AndersenHandle AH) const {
  EnumerationState *ES = AH;
  return AndersenEnumerator(ES);
}

const PointsToSet *AndersenPass::getPointsToSetContentsSoFar(AndersenHandle AH)
    const {
  EnumerationState *ES = AH;
  if (!ES) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  return &ES->getElementsSoFar();
}

AndersenEnumerator AndersenPass::enumeratePointsToSetContentsRemaining(
    AndersenHandle AH) const {
  EnumerationState *ES = AH;
  return enumerateRemaining(ES);
}

bool AndersenPass::runOnModule(Module &M) {
  assert(!Data);
  Data = InstructionAnalyzer::run(this, M);
  if (NonLazy) {
    for (ValueInfoMap::const_iterator i = Data->ValueInfos.begin();
         i != Data->ValueInfos.end(); ++i) {
      DEBUG_WITH_TYPE("andersen-order", dbgs() << i->first << '\n');
      getPointsToSet(getHandleToPointsToSet(i->first));
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
  writeEquations(Data, OS);
}

}

namespace llvm {
namespace andersen_internal {

// Exclusively for calling in the debugger.
void dumpAnalysis(const Data *Data) {
  const char *Filename = "andersen.txt";
  errs() << "Dumping analysis to '" << Filename << "'...";

  std::string ErrorInfo;
  raw_fd_ostream File(Filename, ErrorInfo);

  if (ErrorInfo.empty()) {
    writeEquations(Data, File);
  } else {
    errs() << "  error opening file for writing!";
  }
  errs() << "\n";
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
