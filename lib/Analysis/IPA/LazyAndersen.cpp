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
#include "LazyAndersenEnumerator.h"
#include "LazyAndersenGraphTraits.h"
#include "LazyAndersenInstructionAnalyzer.h"
#include "LazyAndersenValuePrinter.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

using namespace llvm;
using namespace llvm::lazyandersen;

cl::opt<bool> NonLazy("andersen-non-lazy",
                      cl::desc("Perform Andersen analysis non-lazily"));
cl::opt<bool> GraphRelations("andersen-graph-relations",
                             cl::desc("Display the Andersen relations graph"));

char LazyAndersen::ID = 0;
// TODO: What do these two bools mean?
INITIALIZE_PASS(LazyAndersen, "lazy-andersen",
                "Lazy Andersen's Algorithm for Points-To Analysis", false,
                true)

LazyAndersen::LazyAndersen()
  : ModulePass(ID), Data(0) {
  initializeLazyAndersenPass(*PassRegistry::getPassRegistry());
}

DenseSet<const Value *> LazyAndersen::getPointsToSet(const Value *V) const {
  DenseSet<const Value *> Out;
  if (Data->ValueInfos[V]) {
    Enumerator::enumerate(
        Data->ValueInfos[V]->getAlgorithmResult<POINTS_TO_SET>(), &Out);
  }
  return Out;
}

bool LazyAndersen::runOnModule(Module &M) {
  assert(!Data);
  Data = InstructionAnalyzer::run(M);
  if (NonLazy) {
    for (ValueInfo::Map::const_iterator i = Data->ValueInfos.begin();
         i != Data->ValueInfos.end(); ++i) {
      getPointsToSet(i->first);
    }
  }
  if (GraphRelations) {
    lazyandersen::viewLazyAndersenGraph(Data, &M);
  }
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
  if (!NonLazy) {
    OS << "Nothing to print. Re-run with -andersen-non-lazy to see points-to"
          " analysis results.\n";
    return;
  }
  for (ValueInfo::Map::const_iterator i = Data->ValueInfos.begin();
       i != Data->ValueInfos.end(); ++i) {
    DenseSet<const Value *> PointsTo(getPointsToSet(i->first));
    OS << "Points-to set for " << prettyPrintValue(i->first) << ":\n";
    for (DenseSet<const Value *>::iterator i = PointsTo.begin();
         i != PointsTo.end(); ++i) {
      OS << "  " << prettyPrintValue(*i) << '\n';
    }
  }
}
