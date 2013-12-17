//===-- Andersen.cpp -----------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the common initialization routines for the Andersen
// library.
//
//===----------------------------------------------------------------------===//

#include "llvm/InitializePasses.h"
#include "llvm/PassRegistry.h"
#include "llvm-c/Initialization.h"

using namespace llvm;

/// initializeAndersen - Initialize all passes linked into the Andersen library.
void llvm::initializeAndersen(PassRegistry &Registry) {
  initializeAndersenAliasAnalysisPass(Registry);
  initializeAndersenGraphPrinterPass(Registry);
  initializeAndersenGraphViewerPass(Registry);
  initializeAndersenPassPass(Registry);
}

void LLVMInitializeAndersen(LLVMPassRegistryRef R) {
  initializeAndersen(*unwrap(R));
}
