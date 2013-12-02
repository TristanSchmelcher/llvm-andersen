//===- LazyAndersenInstructionAnalyzer.cpp - instruction analysis ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the type for instruction analysis.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenInstructionAnalyzer.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenData.h"
#include "LazyAndersenRelationHandler.h"
#include "LazyAndersenPointsToAlgorithm.h"
#include "LazyAndersenRelationType.h"
#include "LazyAndersenValueInfo.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Value.h"
#include "llvm/Pass.h"

using namespace llvm;
using namespace llvm::lazyandersen;

LazyAndersenData *InstructionAnalyzer::run(ModulePass *MP, Module &M) {
  return InstructionAnalyzer(MP, M).Data;
}

void InstructionAnalyzer::visitReturnInst(ReturnInst &I) {
  const Value *ReturnValue = I.getReturnValue();
  if (!ReturnValue) {
    return;
  }
  ValueInfo *ReturnedValueInfo = analyzeValue(ReturnValue);
  if (ReturnedValueInfo) {
    ValueInfo *FunctionValueInfo = getGlobalRegionInfo(CurrentFunction);
    RelationHandler::handleRelation<RETURNED_TO_CALLER>(ReturnedValueInfo,
        FunctionValueInfo);
  }
}

void InstructionAnalyzer::visitAllocaInst(AllocaInst &I) {
  cacheNewRegion(&I);
}

void InstructionAnalyzer::visitLoadInst(LoadInst &I) {
  ValueInfo *AddressAnalysis = analyzeValue(I.getPointerOperand());
  if (AddressAnalysis) {
    ValueInfo *LoadedValueInfo = cacheNewValueInfo(&I);
    RelationHandler::handleRelation<LOADED_FROM>(LoadedValueInfo,
        AddressAnalysis);
  } else {
    cacheNil(&I);
  }
  // TODO: Record that the current function may load this address.
}

void InstructionAnalyzer::visitStoreInst(StoreInst &I) {
  ValueInfo *AddressAnalysis = analyzeValue(I.getPointerOperand());
  if (AddressAnalysis) {
    ValueInfo *StoredValueInfo = analyzeValue(I.getValueOperand());
    if (StoredValueInfo) {
      RelationHandler::handleRelation<STORED_TO>(StoredValueInfo,
          AddressAnalysis);
      // TODO: Record that the current function may store this address.
    }
  }
}

void InstructionAnalyzer::visitAtomicCmpXchgInst(AtomicCmpXchgInst &I) {
  // Since Andersen's algorithm is flow-insensitive, atomicity doesn't matter.
  // The effect of AtomicCmpXchgInst is the same as the equivalent non-atomic
  // pseudo-code:
  //
  // old = *PointerOperand
  // if (old == CompareOperand) {
  //   *PointerOperand = NewValOperand;
  // }
  // return old;  
  //
  // which is just a load relation and a store relation.
  ValueInfo *AddressAnalysis = analyzeValue(I.getPointerOperand());
  if (AddressAnalysis) {
    ValueInfo *LoadedValueInfo = cacheNewValueInfo(&I);
    RelationHandler::handleRelation<LOADED_FROM>(LoadedValueInfo,
        AddressAnalysis);
    ValueInfo *StoredValueInfo = analyzeValue(I.getNewValOperand());
    if (StoredValueInfo) {
      RelationHandler::handleRelation<STORED_TO>(StoredValueInfo,
          AddressAnalysis);
    }
  } else {
    cacheNil(&I);
  }
  // TODO: Record that the current function may load and store this address.
}

void InstructionAnalyzer::visitAtomicRMWInst(AtomicRMWInst &I) {
  // Similarly, the effect is the same as the equivalent non-atomic pseudo-code:
  //
  // old = *PointerOperand
  // new = mix_bits(old, ValOperand)
  // *PointerOperand = new
  // return old
  //
  // From an Andersen perspective, this is the same as simply clobbering the
  // value with ValOperand, so the effect is the same as in AtomicCmpXchgInst.
  ValueInfo *AddressAnalysis = analyzeValue(I.getPointerOperand());
  if (AddressAnalysis) {
    ValueInfo *LoadedValueInfo = cacheNewValueInfo(&I);
    RelationHandler::handleRelation<LOADED_FROM>(LoadedValueInfo,
        AddressAnalysis);
    ValueInfo *StoredValueInfo = analyzeValue(I.getValOperand());
    if (StoredValueInfo) {
      RelationHandler::handleRelation<STORED_TO>(StoredValueInfo,
          AddressAnalysis);
    }
  } else {
    cacheNil(&I);
  }
}

void InstructionAnalyzer::visitPHINode(PHINode &I) {
  ValueInfo *PHIAnalysis = cacheNewValueInfo(&I);
  PHINodeWork.push_back(PHINodeWorkVector::value_type(&I, PHIAnalysis));
}

void InstructionAnalyzer::visitVAStartInst(VAStartInst &I) {
  // Since the va_list layout is target-dependent and not everything accesses it
  // with va_arg instructions, we play it safe by assuming that a value loaded
  // from the va_list may potentially alias the arguments or the va_list itself.
  // Thus multiply-indirect loads are treated as potentially aliasing them too.
  ValueInfo *ArgumentsVI = createAnonymousValueInfo();
  RelationHandler::handleRelation<ARGUMENT_FROM_CALLER>(ArgumentsVI,
      getGlobalRegionInfo(CurrentFunction));
  ValueInfo *VAListVI = analyzeValue(I.getArgList());
  if (!VAListVI) return;
  // Pretend that va_start stores the va_list address to itself.
  RelationHandler::handleRelation<STORED_TO>(VAListVI, VAListVI);
  // Pretend that va_start stores the arguments to the va_list.
  RelationHandler::handleRelation<STORED_TO>(ArgumentsVI, VAListVI);
}

void InstructionAnalyzer::visitVAEndInst(VAEndInst &I) {
  // No-op (va_end just deallocates stuff).
}

void InstructionAnalyzer::visitVACopyInst(VACopyInst &I) {
  // Assume that a value loaded from the destination va_list may potentially
  // alias the arguments, the destination va_list itself, or the source va_list
  // (and any other va_lists from which it was copied).
  ValueInfo *SrcVI = analyzeValue(I.getSrc());
  ValueInfo *DestVI = analyzeValue(I.getDest());
  if (!SrcVI || !DestVI) return;
  // Pretend that va_copy copies the va_list contents.
  ValueInfo *ContentsVI = createAnonymousValueInfo();
  RelationHandler::handleRelation<LOADED_FROM>(ContentsVI, SrcVI);
  RelationHandler::handleRelation<STORED_TO>(ContentsVI, DestVI);
  // Pretend that it also writes the destination va_list's address to itself.
  RelationHandler::handleRelation<STORED_TO>(DestVI, DestVI);
}

void InstructionAnalyzer::visitVAArgInst(VAArgInst &I) {
  // TODO: With the help of a special relation, we could model that this can
  // only return arguments and not the va_list address.
  ValueInfo *VAListVI = analyzeValue(I.getPointerOperand());
  if (!VAListVI) {
    cacheNil(&I);
    return;
  }
  ValueInfo *VAArgVI = cacheNewValueInfo(&I);
  // Pretend that va_arg just reads from the va_list, returning either an
  // argument or the va_list itself.
  RelationHandler::handleRelation<LOADED_FROM>(VAArgVI, VAListVI);
}

void InstructionAnalyzer::visitInstruction(Instruction &I) {
  assert(!I.mayReadOrWriteMemory() && "Unhandled memory instruction");
  analyzeUser(&I);
}

void InstructionAnalyzer::visitFenceInst(FenceInst &I) {
  // No-op (can't use default visitInstruction because it fails the assert).
}

void InstructionAnalyzer::visitMemSetInst(MemSetInst &I) {
  // Equivalent to a write.
  ValueInfo *DestVI = analyzeValue(I.getRawDest());
  ValueInfo *ValueVI = analyzeValue(I.getValue());
  if (!DestVI || !ValueVI) return;
  RelationHandler::handleRelation<STORED_TO>(ValueVI, DestVI);
}

void InstructionAnalyzer::visitMemCpyInst(MemCpyInst &I) {
  visitMemTransferInst(I);
}

void InstructionAnalyzer::visitMemMoveInst(MemMoveInst &I) {
  visitMemTransferInst(I);
}

void InstructionAnalyzer::visitMemTransferInst(MemTransferInst &I) {
  // Equivalent to a load and store.
  ValueInfo *DestVI = analyzeValue(I.getRawDest());
  ValueInfo *SrcVI = analyzeValue(I.getRawSource());
  if (!DestVI || !SrcVI) return;
  ValueInfo *LoadVI = createAnonymousValueInfo();
  RelationHandler::handleRelation<LOADED_FROM>(LoadVI, SrcVI);
  RelationHandler::handleRelation<STORED_TO>(LoadVI, DestVI);
}

InstructionAnalyzer::InstructionAnalyzer(ModulePass *MP, Module &M)
  : MP(MP), Data(createLazyAndersenData()) {
  for (Module::iterator i = M.begin(), End = M.end(); i != End; ++i) {
    Function &F(*i);
    if (!F.isDeclaration()) {
      processFunction(F);
    }
    analyzeValue(&F);
  }
  for (Module::global_iterator i = M.global_begin(), End = M.global_end();
       i != End; ++i) {
    analyzeValue(&*i);
  }
  for (Module::alias_iterator i = M.alias_begin(), End = M.alias_end();
       i != End; ++i) {
    analyzeValue(&*i);
  }
}

LazyAndersenData *InstructionAnalyzer::createLazyAndersenData() {
  // All global regions that are externally accessible by way of linkage. This
  // is the set of all internally-defined global regions with external linkage
  // plus a placeholder for any externally-defined global regions, which is
  // needed to ensure the points-to set is non-empty in the unlikely event that
  // there are no internally-defined global regions with external linkage in
  // this module. We simply create this as a region so as to use itself as the
  // placeholder.
  ValueInfo *ExternallyLinkableRegions = createRegion(0);

  // All regions that are externally accessible in any manner. This is the
  // members of the above set plus all internally-defined regions that can be
  // accessed by dereferencing or calling them. (In this context, the
  // placeholder created above also represents externally-defined non-global
  // regions, which are indistinguishable.)
  ValueInfo *ExternallyAccessibleRegions = createValueInfo(0);
  RelationHandler::handleRelation<DEPENDS_ON>(ExternallyAccessibleRegions,
      ExternallyLinkableRegions);
  // Putting ExternallyAccessibleRegions into every relation with itself makes
  // it expand to what we want.
  RelationHandler::handleRelation<ARGUMENT_FROM_CALLER>(
      ExternallyAccessibleRegions, ExternallyAccessibleRegions);
  RelationHandler::handleRelation<ARGUMENT_TO_CALLEE>(
      ExternallyAccessibleRegions, ExternallyAccessibleRegions);
  RelationHandler::handleRelation<LOADED_FROM>(
      ExternallyAccessibleRegions, ExternallyAccessibleRegions);
  RelationHandler::handleRelation<RETURNED_FROM_CALLEE>(
      ExternallyAccessibleRegions, ExternallyAccessibleRegions);
  RelationHandler::handleRelation<RETURNED_TO_CALLER>(
      ExternallyAccessibleRegions, ExternallyAccessibleRegions);
  RelationHandler::handleRelation<STORED_TO>(
      ExternallyAccessibleRegions, ExternallyAccessibleRegions);
  RelationHandler::handleRelation<ARGUMENT_FROM_CALLER>(
      ExternallyAccessibleRegions, ExternallyAccessibleRegions);

  return new LazyAndersenData(ExternallyLinkableRegions,
                              ExternallyAccessibleRegions);
}

void InstructionAnalyzer::processFunction(Function &F) {
  CurrentFunction = &F;
  // Visit the basic blocks in a depth-first traversal of the dominator tree.
  // This ensures that we visit each instruction before each non-PHI use of it.
  DomTreeNode *Root = MP->getAnalysis<DominatorTree>(F).getRootNode();
  for (df_iterator<DomTreeNode *> i = df_begin(Root), End = df_end(Root);
       i != End; ++i) {
    visit(i->getBlock());
  }
  // Process all the PHI nodes.
  for (PHINodeWorkVector::const_iterator i = PHINodeWork.begin();
       i != PHINodeWork.end(); ++i) {
    const PHINode *PHI = i->first;
    ValueInfo *PHIAnalysis = i->second;
    for (PHINode::const_op_iterator i = PHI->op_begin(); i != PHI->op_end();
         ++i) {
      ValueInfo *OperandAnalysis = analyzeValue(*i);
      if (OperandAnalysis) {
        RelationHandler::handleRelation<DEPENDS_ON>(PHIAnalysis,
            OperandAnalysis);
      }
    }
  }
  PHINodeWork.clear();
}

void InstructionAnalyzer::visitCallSite(CallSite CS) {
  const Value *CalledValue = CS.getCalledValue();
  ValueInfo *CalledValueInfo = analyzeValue(CalledValue);
  if (CalledValueInfo) {
    for (CallSite::arg_iterator i = CS.arg_begin(), End = CS.arg_end();
         i != End; ++i) {
      const Value *ArgumentValue = *i;
      ValueInfo *ArgumentValueInfo = analyzeValue(ArgumentValue);
      if (ArgumentValueInfo) {
        RelationHandler::handleRelation<ARGUMENT_TO_CALLEE>(ArgumentValueInfo,
            CalledValueInfo);
      }
    }
  }
  if (!CS.getType()->isVoidTy()) {
    if (CalledValueInfo) {
      ValueInfo *ReturnedValueInfo = cacheNewValueInfo(CS.getInstruction());
      RelationHandler::handleRelation<RETURNED_FROM_CALLEE>(ReturnedValueInfo,
          CalledValueInfo);
    } else {
      cacheNil(CS.getInstruction());
    }
  }
  // TODO: Record that the current function may call this function.
}

bool InstructionAnalyzer::analyzed(const Value *V) {
  return Data->ValueInfos.count(V);
}

ValueInfo *InstructionAnalyzer::makeRegion(ValueInfo *VI) {
  VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()
      ->addValueInfo(VI);
  return VI;
}

ValueInfo *InstructionAnalyzer::createValueInfo(const Value *V) {
  return new ValueInfo(V);
}

ValueInfo *InstructionAnalyzer::createRegion(const Value *V) {
  return makeRegion(createValueInfo(V));
}

ValueInfo *InstructionAnalyzer::cache(const Value *V, ValueInfo *VI) {
  assert(V);
  assert(!analyzed(V));
  Data->ValueInfos[V] = VI;
  return VI;
}

ValueInfo *InstructionAnalyzer::cacheNewValueInfo(const Value *V) {
  return cache(V, createValueInfo(V));
}

ValueInfo *InstructionAnalyzer::cacheNewRegion(const Value *V) {
  return makeRegion(cacheNewValueInfo(V));
}

ValueInfo *InstructionAnalyzer::cacheNil(const Value *V) {
  return cache(V, ValueInfo::Nil);
}

ValueInfo *InstructionAnalyzer::getGlobalRegionInfo(const GlobalValue *G) {
  assert(!G->isDeclaration());
  assert(!isa<GlobalAlias>(G));
  ValueInfo::Ref &VI = Data->GlobalRegionInfos[G];
  ValueInfo *Out;
  if (VI) {
    Out = VI.getPtr();
  } else {
    Out = createRegion(G);
    VI = Out;
  }
  return Out;
}

ValueInfo *InstructionAnalyzer::createAnonymousValueInfo() {
  ValueInfo *VI = createValueInfo(0);
  Data->AnonymousValueInfos.push_back(VI);
  return VI;
}

ValueInfo *InstructionAnalyzer::analyzeValue(const Value *V) {
  assert(V);
  ValueInfoMap::const_iterator i = Data->ValueInfos.find(V);
  if (i != Data->ValueInfos.end()) {
    // Previously analyzed.
    return i->second.getPtr();
  }
  // Else analyze now.
  ValueInfo *VI;
  if (const GlobalValue *G = dyn_cast<GlobalValue>(V)) {
    VI = analyzeGlobalValue(G);
  } else if (const Argument *A = dyn_cast<Argument>(V)) {
    VI = analyzeArgument(A);
  } else if (const Instruction *I = dyn_cast<Instruction>(V)) {
    // Since we visit BBs in control-flow order, this can only happen if a
    // reachable BB has a PHI node with an incoming value from an unreachable
    // BB. Since the instruction cannot possibly execute, we can pretend that
    // its result points to nothing.
    assert(!MP->getAnalysis<DominatorTree>(*CurrentFunction)
               .isReachableFromEntry(I->getParent()) &&
           "Instruction used before executed");
    VI = cacheNil(V);
  } else if (const User *U = dyn_cast<User>(V)) {
    VI = analyzeUser(U);
  } else {
    // TODO: Are there other types of Values that can point to things?
    VI = cacheNil(V);
  }
  return VI;
}

ValueInfo *InstructionAnalyzer::analyzeGlobalValue(const GlobalValue *G) {
  if (G->isDeclaration()) {
    assert(!G->hasLocalLinkage());  // Verifier ensures this
    return cache(G, Data->ExternallyLinkableRegions.getPtr());
  } else {
    ValueInfo *VI;
    if (const GlobalAlias *GA = dyn_cast<GlobalAlias>(G)) {
      VI = analyzeGlobalAlias(GA);
    } else {
      assert(isa<GlobalVariable>(G) || isa<Function>(G));
      VI = analyzeGlobalRegion(G);
    }
    if (VI != ValueInfo::Nil && !G->hasLocalLinkage()) {
      RelationHandler::handleRelation<DEPENDS_ON>(
          Data->ExternallyLinkableRegions.getPtr(), VI);
    }
    return VI;
  }
}

ValueInfo *InstructionAnalyzer::analyzeGlobalAlias(const GlobalAlias *GA) {
  const Constant *Aliasee = GA->getAliasee();
  if (Aliasee) {
    // Even when not overridable, we have to make a new ValueInfo because
    // alias chains can have cycles.
    ValueInfo *VI = cacheNewValueInfo(GA);
    RelationHandler::handleRelation<DEPENDS_ON>(VI, analyzeValue(Aliasee));
    if (GA->mayBeOverridden()) {
      RelationHandler::handleRelation<DEPENDS_ON>(
          VI, Data->ExternallyLinkableRegions.getPtr());
    }
    return VI;
  } else if (GA->mayBeOverridden()) {
    // TODO: What does it mean for an alias to alias nothing?
    return cache(GA, Data->ExternallyLinkableRegions.getPtr());
  } else {
    // TODO: What does it mean for an alias to alias nothing?
    return cacheNil(GA);
  }
}

ValueInfo *InstructionAnalyzer::analyzeGlobalRegion(const GlobalValue *G) {
  ValueInfo *RegionVI = getGlobalRegionInfo(G);
  ValueInfo *VI;
  if (G->mayBeOverridden()) {
    // It either points to this region or an externally-linkable region.
    VI = cacheNewValueInfo(G);
    RelationHandler::handleRelation<DEPENDS_ON>(VI, RegionVI);
    RelationHandler::handleRelation<DEPENDS_ON>(
        VI, Data->ExternallyLinkableRegions.getPtr());
  } else {
    // It can only point to this region.
    VI = cache(G, RegionVI);
  }
  if (const GlobalVariable *GV = dyn_cast<GlobalVariable>(G)) {
    assert(GV->hasInitializer());
    ValueInfo *InitializerValueInfo = analyzeValue(GV->getInitializer());
    if (InitializerValueInfo) {
      // Since Andersen's algorithm is flow-insensitive, the effect of an
      // initializer is the same as that of a store instruction, except that it
      // can only store to the region defined in this module.
      RelationHandler::handleRelation<STORED_TO>(InitializerValueInfo,
          RegionVI);
    }
  }
  return VI;
}

ValueInfo *InstructionAnalyzer::analyzeArgument(const Argument *A) {
  ValueInfo *ArgumentValueInfo = cacheNewValueInfo(A);
  ValueInfo *FunctionValueInfo = getGlobalRegionInfo(CurrentFunction);
  RelationHandler::handleRelation<ARGUMENT_FROM_CALLER>(ArgumentValueInfo,
      FunctionValueInfo);
  return ArgumentValueInfo;
}

ValueInfo *InstructionAnalyzer::analyzeUser(const User *U) {
  typedef SmallVector<ValueInfo *, 3> ValueInfoVector;
  ValueInfoVector Set;
  for (User::const_op_iterator i = U->op_begin(); i != U->op_end(); ++i) {
    ValueInfo *VI = analyzeValue(*i);
    if (VI != ValueInfo::Nil) {
      Set.push_back(VI);
    }
  }
  ValueInfo *Result;
  switch (Set.size()) {
  case 0:
    Result = cacheNil(U);
    break;
  case 1:
    Result = cache(U, Set.front());
    break;
  default:
    Result = cacheNewValueInfo(U);
    for (ValueInfoVector::const_iterator i = Set.begin(); i != Set.end(); ++i) {
      RelationHandler::handleRelation<DEPENDS_ON>(Result, *i);
    }
    break;
  }
  return Result;
}
