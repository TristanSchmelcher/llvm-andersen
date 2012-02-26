//===- LazyAndersen.cpp - Lazy Andersen's algorithm -------------*- C++ -*-===//
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
#include <stdio.h>
using namespace llvm;

char LazyAndersen::ID = 0;
// TODO: What do these two bools mean?
INITIALIZE_PASS(LazyAndersen, "lazy-andersen",
                "Lazy Andersen's Algorithm for Points-To Analysis", false, true)

LazyAndersen::LazyAndersen()
  : ModulePass(ID) {
  initializeLazyAndersenPass(*PassRegistry::getPassRegistry());
}

bool LazyAndersen::runOnModule(Module &M) {
  // TODO
  printf("foo\n");
  return false;
}

void LazyAndersen::releaseMemory() {
  // TODO
}

void LazyAndersen::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

void LazyAndersen::print(raw_ostream &OS, const Module *) const {
  // TODO
}

