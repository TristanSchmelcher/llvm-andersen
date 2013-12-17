//===- AndersenAliasAnalysis.cpp - Andersen-based alias analysis ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the AndersenAliasAnalysis pass, which is an alias
// analysis implementation that uses AndersenPass queries.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "andersen-aa"
#include "ValueInfo.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AndersenEnumerator.h"
#include "llvm/Analysis/AndersenPass.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"
#include "llvm/Pass.h"

namespace llvm {
namespace {

using namespace andersen_internal;

/// AndersenAliasAnalysis - An alias analysis implementation that uses
/// AndersenPass queries.
class AndersenAliasAnalysis : public ModulePass, public AliasAnalysis {
  AndersenPass *AP;

public:
  static char ID; // Class identification, replacement for typeinfo
  AndersenAliasAnalysis() : ModulePass(ID), AP(0) {
    initializeAndersenAliasAnalysisPass(*PassRegistry::getPassRegistry());
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

char AndersenAliasAnalysis::ID = 0;

void AndersenAliasAnalysis::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequiredTransitive<AndersenPass>();
  AU.setPreservesAll();
  AliasAnalysis::getAnalysisUsage(AU);
}

bool AndersenAliasAnalysis::runOnModule(Module &M) {
  InitializeAliasAnalysis(this);
  AP = &getAnalysis<AndersenPass>();
  return false;
}

AliasAnalysis::AliasResult
AndersenAliasAnalysis::alias(const Location &LocA,
                             const Location &LocB) {
  const PointsToSet *PointsToSetA = AP->getPointsToSet(LocA.Ptr);
  if (!PointsToSetA) {
    return NoAlias;
  }
  for (AndersenEnumerator AE(AP->enumeratePointsToSet(LocB.Ptr));; ) {
    ValueInfo *Next = AE.enumerate();
    if (!Next) break;
    if (PointsToSetA->count(Next)) {
      // MayAlias as far as we can tell. Chain to next AliasAnalysis.
      return AliasAnalysis::alias(LocA, LocB);
    }
  }
  // No overlap in the points-to sets, so cannot alias.
  return NoAlias;
}

bool AndersenAliasAnalysis::pointsToConstantMemory(const Location &Loc,
                                                   bool OrLocal) {
  // This is loosely based on the BasicAliasAnalysis implementation.
  for (AndersenEnumerator AE(AP->enumeratePointsToSet(Loc.Ptr));; ) {
    ValueInfo *Next = AE.enumerate();
    if (!Next) break;
    const Value *V = Next->getValue();
    if (!V) {
      // External regions can't be guaranteed to be either const or local.
      // TODO: We can do better for overridable GlobalVariables if we introduce
      // a special VI for externally linkable constant regions, since it isn't
      // legal for a global to be marked constant in some modules and
      // non-constant in others
      return AliasAnalysis::pointsToConstantMemory(Loc, OrLocal);
    }

    if (OrLocal && isa<AllocaInst>(V)) {
      continue;
    }

    if (const GlobalVariable *GV = dyn_cast<GlobalVariable>(V)) {
      if (GV->isConstant()) {
        continue;
      }
    }

    return AliasAnalysis::pointsToConstantMemory(Loc, OrLocal);
  }
  return true;
}

void AndersenAliasAnalysis::deleteValue(Value *V) {
  // TODO
  AliasAnalysis::deleteValue(V);
}

void AndersenAliasAnalysis::copyValue(Value *From, Value *To) {
  // TODO
  AliasAnalysis::copyValue(From, To);
}

void AndersenAliasAnalysis::addEscapingUse(Use &U) {
  // TODO
  AliasAnalysis::addEscapingUse(U);
}

AliasAnalysis::ModRefResult
AndersenAliasAnalysis::getModRefInfo(ImmutableCallSite CS,
                                     const Location &Loc) {
  // TODO
  // Note: the base class implementation does more than just forward. Ideally
  // it should expose an accessor for the next AliasAnalysis in the chain.
  return AliasAnalysis::getModRefInfo(CS, Loc);
}

AliasAnalysis::ModRefResult
AndersenAliasAnalysis::getModRefInfo(ImmutableCallSite CS1,
                                     ImmutableCallSite CS2) {
  // TODO
  // Note: the base class implementation does more than just forward. Ideally
  // it should expose an accessor for the next AliasAnalysis in the chain.
  return AliasAnalysis::getModRefInfo(CS1, CS2);
}

AliasAnalysis::ModRefBehavior
AndersenAliasAnalysis::getModRefBehavior(ImmutableCallSite CS) {
  // TODO
  // Note: the base class implementation does more than just forward. Ideally
  // it should expose an accessor for the next AliasAnalysis in the chain.
  return AliasAnalysis::getModRefBehavior(CS);
}

AliasAnalysis::ModRefBehavior
AndersenAliasAnalysis::getModRefBehavior(const Function *F) {
  // TODO
  return AliasAnalysis::getModRefBehavior(F);
}

}
}

llvm::ModulePass *llvm::createAndersenAliasAnalysisPass() {
  return new AndersenAliasAnalysis();
}

using namespace llvm;

// TODO: What do these three bools mean?
INITIALIZE_AG_PASS_BEGIN(AndersenAliasAnalysis, AliasAnalysis,
                         "andersen-aa",
                         "Andersen-based Alias Analysis", false, true,
                         false)
INITIALIZE_PASS_DEPENDENCY(AndersenPass)
INITIALIZE_AG_PASS_END(AndersenAliasAnalysis, AliasAnalysis,
                       "andersen-aa", "Andersen-based Alias Analysis",
                       false, true, false)
