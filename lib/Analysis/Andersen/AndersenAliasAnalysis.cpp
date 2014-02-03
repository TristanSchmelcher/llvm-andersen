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

#include "ValueInfo.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AndersenEnumerator.h"
#include "llvm/Analysis/AndersenPass.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Casting.h"
#include "llvm/Pass.h"

#include <cassert>

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
  ModRefBehavior getFunctionPointerModRefBehavior(const Value *F);
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
  AndersenSetHandle A = AP->getHandleToPointsToSet(
      AP->getHandleToValue(LocA.Ptr));
  AndersenSetHandle B = AP->getHandleToPointsToSet(
      AP->getHandleToValue(LocB.Ptr));
  if (AP->isSetIntersectionEmpty(A, B)) {
    return NoAlias;
  } else {
    return AliasAnalysis::alias(LocA, LocB);
  }
}

bool AndersenAliasAnalysis::pointsToConstantMemory(const Location &Loc,
                                                   bool OrLocal) {
  AndersenSetHandle L = AP->getHandleToPointsToSet(
      AP->getHandleToValue(Loc.Ptr));
  // This is loosely based on the BasicAliasAnalysis implementation.
  for (AndersenEnumerator AE(AP->enumerateSet(L));; ) {
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
  ModRefResult Result = NoModRef;
  AndersenValueHandle F = AP->getHandleToValue(CS.getCalledValue());
  AndersenSetHandle Reads = AP->getHandleToFunctionPointerReadSet(F);
  AndersenSetHandle Writes = AP->getHandleToFunctionPointerWriteSet(F);
  AndersenSetHandle Location = AP->getHandleToPointsToSet(
      AP->getHandleToValue(Loc.Ptr));
  if (!AP->isSetIntersectionEmpty(Reads, Location)) {
    Result = ModRefResult(Result | Ref);
  }
  if (!AP->isSetIntersectionEmpty(Writes, Location)) {
    Result = ModRefResult(Result | Mod);
  }
  if (Result != NoModRef) {
    Result = ModRefResult(Result & AliasAnalysis::getModRefInfo(CS, Loc));
  }
  return Result;
}

AliasAnalysis::ModRefResult
AndersenAliasAnalysis::getModRefInfo(ImmutableCallSite CS1,
                                     ImmutableCallSite CS2) {
  // From the documentation, the semantics of this call are:
  // - NoModRef if neither call writes to memory read or written by the other.
  // - Ref if CS1 reads memory written by CS2.
  // - Mod if CS1 writes to memory read or written by CS2.
  // - ModRef if CS1 might read or write memory written to by CS2.
  ModRefResult Result = NoModRef;
  AndersenValueHandle F1 = AP->getHandleToValue(CS1.getCalledValue());
  AndersenValueHandle F2 = AP->getHandleToValue(CS2.getCalledValue());
  AndersenSetHandle Reads1 = AP->getHandleToFunctionPointerReadSet(F1);
  AndersenSetHandle Writes1 = AP->getHandleToFunctionPointerWriteSet(F1);
  AndersenSetHandle Writes2 = AP->getHandleToFunctionPointerWriteSet(F2);
  // Fetch of Reads2 is delayed since it may not be needed.
  if (!AP->isSetIntersectionEmpty(Reads1, Writes2)) {
    Result = ModRefResult(Result | Ref);
  }
  if (!AP->isSetIntersectionEmpty(Writes1, Writes2) ||
      !AP->isSetIntersectionEmpty(
          Writes1,
          // Reads2
          AP->getHandleToFunctionPointerReadSet(F2))) {
    Result = ModRefResult(Result | Mod);
  }
  if (Result != NoModRef) {
    Result = ModRefResult(Result & AliasAnalysis::getModRefInfo(CS1, CS2));
  }
  return Result;
}

AliasAnalysis::ModRefBehavior
AndersenAliasAnalysis::getModRefBehavior(ImmutableCallSite CS) {
  ModRefBehavior Result = getFunctionPointerModRefBehavior(CS.getCalledValue());
  if (Result != DoesNotAccessMemory) {
    Result = ModRefBehavior(Result & AliasAnalysis::getModRefBehavior(CS));
  }
  return Result;
}

AliasAnalysis::ModRefBehavior
AndersenAliasAnalysis::getModRefBehavior(const Function *F) {
  ModRefBehavior Result = getFunctionPointerModRefBehavior(F);
  if (Result != DoesNotAccessMemory) {
    Result = ModRefBehavior(Result & AliasAnalysis::getModRefBehavior(F));
  }
  return Result;
}

AliasAnalysis::ModRefBehavior
AndersenAliasAnalysis::getFunctionPointerModRefBehavior(const Value *F) {
  ModRefBehavior Result = DoesNotAccessMemory;
  AndersenValueHandle VH = AP->getHandleToValue(F);
  AndersenSetHandle Reads = AP->getHandleToFunctionPointerReadSet(VH);
  AndersenSetHandle Writes = AP->getHandleToFunctionPointerWriteSet(VH);
  if (!AP->isSetEmpty(Reads)) {
    Result = ModRefBehavior(Result | Anywhere | Ref);
  }
  if (!AP->isSetEmpty(Writes)) {
    Result = ModRefBehavior(Result | Anywhere | Mod);
  }
  return Result;
}

}
}

using namespace llvm;

ModulePass *llvm::createAndersenAliasAnalysisPass() {
  return new AndersenAliasAnalysis();
}

// TODO: What do these three bools mean?
INITIALIZE_AG_PASS_BEGIN(AndersenAliasAnalysis, AliasAnalysis,
                         "andersen-aa",
                         "Andersen-based Alias Analysis", false, true,
                         false)
INITIALIZE_PASS_DEPENDENCY(AndersenPass)
INITIALIZE_AG_PASS_END(AndersenAliasAnalysis, AliasAnalysis,
                       "andersen-aa", "Andersen-based Alias Analysis",
                       false, true, false)
