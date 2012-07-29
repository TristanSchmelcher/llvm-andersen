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

#include "LazyAndersenData.h"
#include "LazyAndersenGraphTraits.h"
#include "LazyAndersenInstructionAnalyzer.h"
#include "llvm/Support/Debug.h"

using namespace llvm;
using namespace llvm::lazyandersen;

char LazyAndersen::ID = 0;
// TODO: What do these two bools mean?
INITIALIZE_PASS(LazyAndersen, "lazy-andersen",
                "Lazy Andersen's Algorithm for Points-To Analysis", false,
                true)

LazyAndersen::LazyAndersen()
  : ModulePass(ID), Data(0) {
  initializeLazyAndersenPass(*PassRegistry::getPassRegistry());
}

bool LazyAndersen::runOnModule(Module &M) {
  assert(!Data);
  Data = InstructionAnalyzer::run(M);
  print(dbgs(), &M);
  return false;
}

void LazyAndersen::releaseMemory() {
  delete Data;
  Data = 0;
}

void LazyAndersen::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

void LazyAndersen::print(raw_ostream &OS, const Module *M) const {
  assert(Data);
  WriteGraph(OS, Data->ValueInfos, false,
      Twine("LazyAndersen analysis results for module ")
          + M->getModuleIdentifier());
#ifndef NDEBUG
  // In debug mode, also display the graph.
  lazyandersen::viewLazyAndersenGraph(Data);
#endif
}
