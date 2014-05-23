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

#include "AnalysisResult.h"
#include "Data.h"
#include "DebugInfo.h"
#include "InstructionAnalyzer.h"
#include "Phase.h"
#include "ModifiedValuesPointsToAlgorithm.h"
#include "PointsToAlgorithm.h"
#include "ReferencedValuesPointsToAlgorithm.h"
#include "TraversalAlgorithm.h"
#include "llvm/Analysis/AndersenEnumerator.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>

namespace llvm {

using namespace andersen_internal;

namespace {

cl::opt<bool> NonLazy("andersen-non-lazy",
                      cl::desc("Perform Andersen analysis non-lazily"));

void writeEquations(const Data *Data, raw_ostream &OS) {
  DebugInfo DI(Data);
  Data->writeEquations(DI, OS);
}

// Get the complete contents for the given set handle, or null if empty. If the
// contents have not yet been fully computed, this method computes it.
const ContentsMap *getContents(AndersenSetHandle SH) {
  AnalysisResult *AR = SH;
  if (!AR) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  // Else it could point to something. Finish any deferred work.
  if (!AR->isDone()) {
    for (AndersenEnumerator AE(AR, AR->getNumValuesEnumeratedSoFar());
         AE.enumerate(); );
    assert(AR->isDone());
  }
  return &AR->getContentsSoFar();
}

}

char AndersenPass::ID = 0;

AndersenPass::AndersenPass()
  : ModulePass(ID), Data(0) {
  initializeAndersenPassPass(*PassRegistry::getPassRegistry());
}

AndersenValueHandle AndersenPass::getHandleToValue(const Value *V) const {
  assert(V);
  ValueInfoMap::const_iterator i = Data->ValueInfos.find(V);
  if (i == Data->ValueInfos.end()) {
    // This can only happen if we are being queried for an unreachable
    // instruction. Pretend its result points to nothing.
    // TODO: Write an assert that verifies this.
    return 0;
  }
  return i->second.getPtr();
}

AndersenSetHandle AndersenPass::getHandleToPointsToSet(AndersenValueHandle VH) {
  ValueInfo *VI = VH;
  if (!VI) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  return VI->getAlgorithmResult<ENUMERATION_PHASE>(PointsToAlgorithm());
}

AndersenSetHandle AndersenPass::getHandleToFunctionPointerReadSet(
    AndersenValueHandle VH) {
  ValueInfo *VI = VH;
  if (!VI) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  return VI->getAlgorithmResult<ENUMERATION_PHASE>(
      TwoHopTraversal<PointsToAlgorithm,
                      ReferencedValuesPointsToAlgorithm>::Algorithm());
}

AndersenSetHandle AndersenPass::getHandleToFunctionPointerWriteSet(
    AndersenValueHandle VH) {
  ValueInfo *VI = VH;
  if (!VI) {
    // We determined this points to nothing at instruction analysis time.
    return 0;
  }
  return VI->getAlgorithmResult<ENUMERATION_PHASE>(
      TwoHopTraversal<PointsToAlgorithm,
                      ModifiedValuesPointsToAlgorithm>::Algorithm());
}

bool AndersenPass::isSetIntersectionEmpty(AndersenSetHandle SH1,
    AndersenSetHandle SH2) {
  if (isSetEmpty(SH1) || isSetEmpty(SH2)) {
    // If either is empty, then we can skip the rest.
    return true;
  }
  // TODO: What is the optimal enumeration strategy?
  const ContentsMap *PointsToSet1 = getContents(SH1);
  assert(PointsToSet1);
  Constraints C;
  for (AndersenEnumerator AE2(enumerateSet(SH2));; ) {
    ValueInfo *Next = AE2.enumerate(&C);
    if (!Next) {
      break;
    }
    ContentsMap::const_iterator i = PointsToSet1->find(Next);
    if (i == PointsToSet1->end()) {
      continue;
    }
    const ConstraintsVector &CV(i->second);
    for (ConstraintsVector::const_iterator j = CV.begin(), End = CV.end();
         j != End; ++j) {
      if (Constraints::areMutuallySatisfiable(C, *j)) {
        return false;
      }
    }
  }
  return true;
}

bool AndersenPass::isSetEmpty(AndersenSetHandle SH) {
  AnalysisResult *AR = SH;
  return AndersenEnumerator(AR).enumerate() == 0;
}

AndersenEnumerator AndersenPass::enumerateSet(AndersenSetHandle SH) {
  AnalysisResult *AR = SH;
  return AndersenEnumerator(AR);
}

bool AndersenPass::runOnModule(Module &M) {
  assert(!Data);
  Data = InstructionAnalyzer::run(this, M);
  if (NonLazy) {
    for (ValueInfoMap::const_iterator i = Data->ValueInfos.begin(),
                                      End = Data->ValueInfos.end();
         i != End; ++i) {
      AndersenValueHandle VH = getHandleToValue(i->first);
      getContents(getHandleToPointsToSet(VH));
      getContents(getHandleToFunctionPointerReadSet(VH));
      getContents(getHandleToFunctionPointerWriteSet(VH));
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
