//===- LazyAndersenAliasAnalysis.cpp - Lazy Andersen-style alias analysis -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the LazyAndersenAliasAnalysis pass, which is an alias
// analysis implementation that uses LazyAndersen queries.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "lazy-andersen-aa"
#include "LazyAndersenTopEnumerator.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/LazyAndersen.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Pass.h"

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  /// LazyAndersenAliasAnalysis - An alias analysis implementation that uses
  /// LazyAndersen queries.
  class LazyAndersenAliasAnalysis : public ModulePass, public AliasAnalysis {
    LazyAndersen *LA;

  public:
    static char ID; // Class identification, replacement for typeinfo
    LazyAndersenAliasAnalysis() : ModulePass(ID), LA(0) {
      initializeLazyAndersenAliasAnalysisPass(*PassRegistry::getPassRegistry());
    }

  private:
    /// getAdjustedAnalysisPointer - This method is used when a pass implements
    /// an analysis interface through multiple inheritance.  If needed, it
    /// should override this to adjust the this pointer as needed for the
    /// specified pass info.
    virtual void *getAdjustedAnalysisPointer(AnalysisID PI) {
      if (PI == &AliasAnalysis::ID)
        return (AliasAnalysis *)this;
      return this;
    }

    virtual void getAnalysisUsage(AnalysisUsage &AU) const;
    virtual bool runOnModule(Module &M);
    virtual AliasResult alias(const Location &LocA, const Location &LocB);
    virtual bool pointsToConstantMemory(const Location &Loc, bool OrLocal);
    virtual void deleteValue(Value *V);
    virtual void copyValue(Value *From, Value *To);
    virtual void addEscapingUse(Use &U);
    virtual ModRefBehavior getModRefBehavior(ImmutableCallSite CS);
    virtual ModRefBehavior getModRefBehavior(const Function *F);
    virtual ModRefResult getModRefInfo(ImmutableCallSite CS,
                                       const Location &Loc);
    virtual ModRefResult getModRefInfo(ImmutableCallSite CS1,
                                       ImmutableCallSite CS2);
  };
}  // End of anonymous namespace

// Register this pass...
char LazyAndersenAliasAnalysis::ID = 0;
// TODO: What do these three bools mean?
INITIALIZE_AG_PASS_BEGIN(LazyAndersenAliasAnalysis, AliasAnalysis,
                         "lazy-andersen-aa",
                         "LazyAndersen-based Alias Analysis", false, true,
                         false)
INITIALIZE_PASS_DEPENDENCY(LazyAndersen)
INITIALIZE_AG_PASS_END(LazyAndersenAliasAnalysis, AliasAnalysis,
                       "lazy-andersen-aa", "LazyAndersen-based Alias Analysis",
                       false, true, false)

ModulePass *llvm::createLazyAndersenAliasAnalysisPass() {
  return new LazyAndersenAliasAnalysis();
}

void LazyAndersenAliasAnalysis::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequiredTransitive<LazyAndersen>();
  AU.setPreservesAll();
  AliasAnalysis::getAnalysisUsage(AU);
}

bool LazyAndersenAliasAnalysis::runOnModule(Module &M) {
  InitializeAliasAnalysis(this);
  LA = &getAnalysis<LazyAndersen>();
  return false;
}

AliasAnalysis::AliasResult
LazyAndersenAliasAnalysis::alias(const Location &LocA,
                                 const Location &LocB) {
  const ValueInfoSetVector *PointsToSetA = LA->getPointsToSet(LocA.Ptr);
  if (!PointsToSetA) {
    return NoAlias;
  }
  for (TopEnumerator TE(LA->enumeratePointsToSet(LocB.Ptr));; ) {
    ValueInfo *Next = TE.enumerate();
    if (!Next) break;
    if (PointsToSetA->count(Next)) {
      // MayAlias as far as we can tell. Chain to next AliasAnalysis.
      return AliasAnalysis::alias(LocA, LocB);
    }
  }
  // No overlap in the points-to sets, so cannot alias.
  return NoAlias;
}

bool LazyAndersenAliasAnalysis::pointsToConstantMemory(const Location &Loc,
                                                       bool OrLocal) {
  // TODO
  return AliasAnalysis::pointsToConstantMemory(Loc, OrLocal);
}

void LazyAndersenAliasAnalysis::deleteValue(Value *V) {
  // TODO
  AliasAnalysis::deleteValue(V);
}

void LazyAndersenAliasAnalysis::copyValue(Value *From, Value *To) {
  // TODO
  AliasAnalysis::copyValue(From, To);
}

void LazyAndersenAliasAnalysis::addEscapingUse(Use &U) {
  // TODO
  AliasAnalysis::addEscapingUse(U);
}

AliasAnalysis::ModRefResult
LazyAndersenAliasAnalysis::getModRefInfo(ImmutableCallSite CS,
                                         const Location &Loc) {
  // TODO
  // Note: the base class implementation does more than just forward. Ideally
  // it should expose an accessor for the next AliasAnalysis in the chain.
  return AliasAnalysis::getModRefInfo(CS, Loc);
}

AliasAnalysis::ModRefResult
LazyAndersenAliasAnalysis::getModRefInfo(ImmutableCallSite CS1,
                                         ImmutableCallSite CS2) {
  // TODO
  // Note: the base class implementation does more than just forward. Ideally
  // it should expose an accessor for the next AliasAnalysis in the chain.
  return AliasAnalysis::getModRefInfo(CS1, CS2);
}

AliasAnalysis::ModRefBehavior
LazyAndersenAliasAnalysis::getModRefBehavior(ImmutableCallSite CS) {
  // TODO
  // Note: the base class implementation does more than just forward. Ideally
  // it should expose an accessor for the next AliasAnalysis in the chain.
  return AliasAnalysis::getModRefBehavior(CS);
}

AliasAnalysis::ModRefBehavior
LazyAndersenAliasAnalysis::getModRefBehavior(const Function *F) {
  // TODO
  return AliasAnalysis::getModRefBehavior(F);
}
