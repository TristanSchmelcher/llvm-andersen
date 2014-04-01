//===- InstructionAnalyzer.cpp - instruction analysis ---------------------===//
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

#include "InstructionAnalyzer.h"

#include "Constraints.h"
#include "Data.h"
#include "RelationHandler.h"
#include "PointsToAlgorithm.h"
#include "ValueInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Value.h"
#include "llvm/InstVisitor.h"
#include "llvm/Pass.h"

#include <cassert>

namespace llvm {
namespace andersen_internal {

class InstructionAnalyzer::Visitor
  : public InstVisitor<InstructionAnalyzer::Visitor> {
  typedef SmallVector<std::pair<const PHINode *, ValueInfo *>, 3>
      PHINodeWorkVector;
  ModulePass *const MP;
  PHINodeWorkVector PHINodeWork;
  Function *CurrentFunction;
  ValueInfo *CurrentFunctionRegion;
  RelationHandler RH;

public:
  static Data *run(ModulePass *MP, Module &M) {
    return Visitor(MP, M).RH.D;
  }

  void visitReturnInst(ReturnInst &I) {
    const Value *ReturnValue = I.getReturnValue();
    if (!ReturnValue) {
      return;
    }
    ValueInfo *ReturnedValueInfo = analyzeValue(ReturnValue);
    if (ReturnedValueInfo) {
      RH.handleRelation<RETURNED_TO_CALLER>(ReturnedValueInfo,
          CurrentFunctionRegion);
    }
  }

  void visitAllocaInst(AllocaInst &I) {
    cacheNewRegion(&I);
  }

  void visitLoadInst(LoadInst &I) {
    ValueInfo *AddressAnalysis = analyzeValue(I.getPointerOperand());
    if (AddressAnalysis) {
      ValueInfo *LoadedValueInfo = cacheNewValueInfo(&I);
      processLoad(LoadedValueInfo, AddressAnalysis);
    } else {
      cacheNil(&I);
    }
  }

  void visitStoreInst(StoreInst &I) {
    ValueInfo *AddressAnalysis = analyzeValue(I.getPointerOperand());
    ValueInfo *StoredValueInfo = analyzeValue(I.getValueOperand());
    if (AddressAnalysis) {
      processStore(StoredValueInfo, AddressAnalysis);
    }
  }

  void visitAtomicCmpXchgInst(AtomicCmpXchgInst &I) {
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
    processAtomicMutateInst(I, I.getPointerOperand(), I.getNewValOperand());
  }

  void visitAtomicRMWInst(AtomicRMWInst &I) {
    // Similarly, the effect is the same as the equivalent non-atomic
    // pseudo-code:
    //
    // old = *PointerOperand
    // new = mix_bits(old, ValOperand)
    // *PointerOperand = new
    // return old
    //
    // From an Andersen perspective, this is the same as simply clobbering the
    // value with ValOperand, so the effect is the same as in AtomicCmpXchgInst.
    processAtomicMutateInst(I, I.getPointerOperand(), I.getValOperand());
  }

  void visitPHINode(PHINode &I) {
    ValueInfo *PHIAnalysis = cacheNewValueInfo(&I);
    PHINodeWork.push_back(PHINodeWorkVector::value_type(&I, PHIAnalysis));
  }

  void visitCallSite(CallSite CS) {
    const Value *CalledValue = CS.getCalledValue();
    ValueInfo *CalledValueInfo = analyzeValue(CalledValue);
    const Instruction *CallSite = CS.getInstruction();
    for (CallSite::arg_iterator i = CS.arg_begin(), End = CS.arg_end();
         i != End; ++i) {
      const Value *ArgumentValue = *i;
      ValueInfo *ArgumentValueInfo = analyzeValue(ArgumentValue);
      if (CalledValueInfo && ArgumentValueInfo) {
        // TODO: This treats all arguments' bits as being mixed together. We
        // should treat each argument position as independent when it is known
        // that the function pointer type matches the function definition. More
        // generally, we should be field-sensitive when feasible.
        RH.handleRelation<ARGUMENT_TO_CALLEE>(ArgumentValueInfo,
            CalledValueInfo, CallSite);
      }
    }
    if (!CS.getType()->isVoidTy()) {
      if (CalledValueInfo) {
        ValueInfo *ReturnedValueInfo = cacheNewValueInfo(CallSite);
        RH.handleRelation<RETURNED_FROM_CALLEE>(ReturnedValueInfo,
            CalledValueInfo, CallSite);
      } else {
        cacheNil(CallSite);
      }
    }
    RH.handleRelation<CALLS>(CurrentFunctionRegion, CalledValueInfo, CallSite);
  }

  void visitVAStartInst(VAStartInst &I) {
    // Since the va_list layout is target-dependent and not everything accesses
    // it with va_arg instructions, we play it safe by assuming that a value
    // loaded from the va_list may potentially alias the arguments or the
    // va_list itself. Thus multiply-indirect loads are treated as potentially
    // aliasing them too.
    ValueInfo *ArgumentsVI = createAnonymousValueInfo();
    RH.handleRelation<ARGUMENT_FROM_CALLER>(ArgumentsVI, CurrentFunctionRegion);
    ValueInfo *VAListVI = analyzeValue(I.getArgList());
    if (VAListVI) {
      // Pretend that va_start stores the va_list address to itself.
      processStore(VAListVI, VAListVI);
      // Pretend that va_start stores the arguments to the va_list.
      processStore(ArgumentsVI, VAListVI);
    }
  }

  void visitVAEndInst(VAEndInst &I) {
    // No-op (va_end just deallocates stuff).
  }

  void visitVACopyInst(VACopyInst &I) {
    // Assume that a value loaded from the destination va_list may potentially
    // alias the arguments, the destination va_list itself, or the source
    // va_list (and any other va_lists from which it was copied).
    ValueInfo *SrcVI = analyzeValue(I.getSrc());
    ValueInfo *DestVI = analyzeValue(I.getDest());
    // If only one of these is true then the operation is unsound, but we try to
    // model it anyway.
    if (DestVI || SrcVI) {
      // Pretend that va_copy copies the va_list contents.
      ValueInfo *ContentsVI = createAnonymousValueInfo();
      if (SrcVI) {
        processLoad(ContentsVI, SrcVI);
      }
      if (DestVI) {
        processStore(ContentsVI, DestVI);
        // Pretend that it also writes the destination va_list's address to
        // itself.
        processStore(DestVI, DestVI);
      }
    }
  }

  void visitVAArgInst(VAArgInst &I) {
    // TODO: With the help of a special relation, we could model that this can
    // only return arguments and not the va_list address.
    ValueInfo *VAListVI = analyzeValue(I.getPointerOperand());
    if (VAListVI) {
      ValueInfo *VAArgVI = cacheNewValueInfo(&I);
      // Pretend that va_arg just reads from the va_list, returning either an
      // argument or the va_list itself.
      processLoad(VAArgVI, VAListVI);
    } else {
      cacheNil(&I);
    }
  }

  void visitInstruction(Instruction &I) {
    assert(!I.mayReadOrWriteMemory() && "Unhandled memory instruction");
    // TODO: This treats the 1-bit result of cmp instructions as aliasing the
    // arguments, which we can't just take out because programmers can do funny
    // things with bit manipulations. To improve on that we would need to
    // calculate the number of bits of information that values have on each
    // possible pointee's address.
    // Note that GEM falls through to here, so the result is treated as
    // depending on the non-pointer arguments too. This means that, e.g.,
    // if a and b are unrelated char pointers then &a[b - a] and b are treated
    // as aliasing each other even though LLVM documents that the behaviour of
    // a[b - a] is undefined.
    analyzeUser(&I);
  }

  void visitFenceInst(FenceInst &I) {
    // No-op (can't use default visitInstruction because it fails the assert).
  }

  void visitMemSetInst(MemSetInst &I) {
    // Equivalent to a write.
    ValueInfo *DestVI = analyzeValue(I.getRawDest());
    ValueInfo *ValueVI = analyzeValue(I.getValue());
    if (DestVI) {
      processStore(ValueVI, DestVI);
    }
  }

  void visitMemTransferInst(MemTransferInst &I) {
    // Equivalent to a load and store.
    ValueInfo *DestVI = analyzeValue(I.getRawDest());
    ValueInfo *SrcVI = analyzeValue(I.getRawSource());
    // If only one of these is true then the operation is unsound, but we try to
    // model it anyway.
    if (DestVI || SrcVI) {
      ValueInfo *LoadVI = createAnonymousValueInfo();
      if (SrcVI) {
        processLoad(LoadVI, SrcVI);
      }
      if (DestVI) {
        processStore(LoadVI, DestVI);
      }
    }
  }

  void visitIntrinsicInst(IntrinsicInst &I) {
    // Delegate special intrinsics that were not already delegated in
    // InstVisitor.
    // TODO: All intrinsics need getModRef handling.
    switch (I.getIntrinsicID()) {
    case Intrinsic::returnaddress:
    case Intrinsic::frameaddress:
      processLowLevelPointerIntrinsic(I);
      break;

    // TODO: Need handling for stacksave/restore to prevent re-ordering with
    // alloca.

    case Intrinsic::not_intrinsic:
      llvm_unreachable("IntrinsicInst not an intrinsic");
    default:
      // All other intrinsics are handled by the CallSite code and treated as
      // calls to external functions.
      InstVisitor<InstructionAnalyzer::Visitor>::visitIntrinsicInst(I);
      break;
    }
  }

private:
  Visitor(ModulePass *MP, Module &M)
    : MP(MP), RH(createData()) {

    // Initialize the special VIs' relations.
    ValueInfo *ExternallyLinkableRegions =
        RH.D->ExternallyLinkableRegions.getPtr();
    ValueInfo *ExternallyAccessibleRegions =
        RH.D->ExternallyAccessibleRegions.getPtr();

    RH.handleRelation<DEPENDS_ON>(ExternallyAccessibleRegions,
        ExternallyLinkableRegions);
    // Putting ExternallyAccessibleRegions into every relation with itself makes
    // it expand to what we want. We use null for the callsites to represent
    // calling from an externally-defined function.
    RH.handleRelation<ARGUMENT_TO_CALLEE>(
        ExternallyAccessibleRegions, ExternallyAccessibleRegions, 0);
    RH.handleRelation<LOADED_FROM>(
        ExternallyAccessibleRegions, ExternallyAccessibleRegions);
    RH.handleRelation<RETURNED_FROM_CALLEE>(
        ExternallyAccessibleRegions, ExternallyAccessibleRegions, 0);
    RH.handleRelation<STORED_TO>(
        ExternallyAccessibleRegions, ExternallyAccessibleRegions);
    // Special case for the function definition argument/return relations: the
    // "dst" VI must be a region. Here ExternallyLinkableRegions represents
    // externally-defined functions.
    RH.handleRelation<ARGUMENT_FROM_CALLER>(
        ExternallyAccessibleRegions, ExternallyLinkableRegions);
    RH.handleRelation<RETURNED_TO_CALLER>(
        ExternallyAccessibleRegions, ExternallyLinkableRegions);
    // Special case for the function definition mod/ref relations: the "src" VI
    // must be a region. Again, ExternallyLinkableRegions represents
    // externally-defined functions.
    RH.handleRelation<CALLS>(
        ExternallyLinkableRegions, ExternallyAccessibleRegions, 0);
    RH.handleRelation<READS_FROM>(
        ExternallyLinkableRegions, ExternallyAccessibleRegions);
    RH.handleRelation<WRITES_TO>(
        ExternallyLinkableRegions, ExternallyAccessibleRegions);

    for (Module::global_iterator i = M.global_begin(), End = M.global_end();
         i != End; ++i) {
      analyzeValue(&*i);
    }
    for (Module::alias_iterator i = M.alias_begin(), End = M.alias_end();
         i != End; ++i) {
      analyzeValue(&*i);
    }
    for (Module::iterator i = M.begin(), End = M.end(); i != End; ++i) {
      Function &F(*i);
      analyzeValue(&F);
      if (!F.isDeclaration()) {
        processFunction(F);
      }
    }
  }

  static Data *createData() {
    // All global regions that are externally accessible by way of linkage. This
    // is the set of all internally-defined global regions with external linkage
    // plus a placeholder for all externally-defined global regions. We simply
    // create this as a region so as to use itself as the placeholder. Unlike
    // normal regions, its points-to set will contain both itself and other
    // VIs.
    ValueInfo *ExternallyLinkableRegions = createRegion(0);

    // All regions that are externally accessible in any manner. This is the
    // members of the above set plus all internally-defined regions that can be
    // accessed by dereferencing or calling them or arguments to
    // externally-defined functions. (In this context, the placeholder created
    // above also represents externally-defined non-global regions, which are
    // indistinguishable.)
    ValueInfo *ExternallyAccessibleRegions = createValueInfo(0);

    return new Data(ExternallyLinkableRegions, ExternallyAccessibleRegions);
  }

  void processLoad(ValueInfo *LoadedValueInfo, ValueInfo *AddressAnalysis) {
    assert(AddressAnalysis);
    assert(LoadedValueInfo);
    RH.handleRelation<LOADED_FROM>(LoadedValueInfo, AddressAnalysis);
    RH.handleRelation<READS_FROM>(CurrentFunctionRegion, AddressAnalysis);
  }

  void processStore(ValueInfo *StoredValueInfo, ValueInfo *AddressAnalysis) {
    assert(AddressAnalysis);
    if (StoredValueInfo) {
      RH.handleRelation<STORED_TO>(StoredValueInfo, AddressAnalysis);
    }
    RH.handleRelation<WRITES_TO>(CurrentFunctionRegion, AddressAnalysis); 
  }

  void processFunction(Function &F) {
    CurrentFunction = &F;
    CurrentFunctionRegion = getGlobalRegionInfo(&F);
    for (Function::arg_iterator i = F.arg_begin(), End = F.arg_end(); i != End;
         ++i) {
      analyzeValue(&*i);
    }
    // Visit the basic blocks in a depth-first traversal of the dominator tree.
    // This ensures that we visit each instruction before each non-PHI use of
    // it.
    DomTreeNode *Root = MP->getAnalysis<DominatorTree>(F).getRootNode();
    for (df_iterator<DomTreeNode *> i = df_begin(Root), End = df_end(Root);
         i != End; ++i) {
      visit(i->getBlock());
    }
    // Process all the PHI nodes.
    for (PHINodeWorkVector::const_iterator i = PHINodeWork.begin(),
                                           End = PHINodeWork.end();
         i != End; ++i) {
      const PHINode *PHI = i->first;
      ValueInfo *PHIAnalysis = i->second;
      for (PHINode::const_op_iterator i = PHI->op_begin(), End = PHI->op_end();
           i != End; ++i) {
        ValueInfo *OperandAnalysis = analyzeValue(*i);
        if (OperandAnalysis) {
          RH.handleRelation<DEPENDS_ON>(PHIAnalysis, OperandAnalysis);
        }
      }
    }
    PHINodeWork.clear();
  }

  bool analyzed(const Value *V) {
    return RH.D->ValueInfos.count(V);
  }

  static ValueInfo *makeRegion(ValueInfo *VI) {
    VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()
        ->addContent(VI, Constraints());
    return VI;
  }

  static ValueInfo *createValueInfo(const Value *V) {
    return new ValueInfo(V);
  }

  static ValueInfo *createRegion(const Value *V) {
    return makeRegion(createValueInfo(V));
  }

  ValueInfo *cache(const Value *V, ValueInfo *VI) {
    assert(V);
    assert(!analyzed(V));
    RH.D->ValueInfos[V] = VI;
    return VI;
  }

  ValueInfo *cacheNewValueInfo(const Value *V) {
    return cache(V, createValueInfo(V));
  }

  ValueInfo *cacheNewRegion(const Value *V) {
    return makeRegion(cacheNewValueInfo(V));
  }

  ValueInfo *cacheNil(const Value *V) {
    return cache(V, 0);
  }

  ValueInfo *getGlobalRegionInfo(const GlobalValue *G) {
    assert(!G->isDeclaration());
    assert(!isa<GlobalAlias>(G));
    ValueInfo::Ref &VI = RH.D->GlobalRegionInfos[G];
    ValueInfo *Out;
    if (VI) {
      Out = VI.getPtr();
    } else {
      Out = createRegion(G);
      VI = Out;
    }
    return Out;
  }

  ValueInfo *createAnonymousValueInfo() {
    ValueInfo *VI = createValueInfo(0);
    RH.D->AnonymousValueInfos.push_back(VI);
    return VI;
  }

  ValueInfo *analyzeValue(const Value *V) {
    assert(V);
    ValueInfoMap::const_iterator i = RH.D->ValueInfos.find(V);
    if (i != RH.D->ValueInfos.end()) {
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
      (void)I;
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

  ValueInfo *analyzeGlobalValue(const GlobalValue *G) {
    if (G->isDeclaration()) {
      assert(!G->hasLocalLinkage());  // Verifier ensures this
      return cache(G, RH.D->ExternallyLinkableRegions.getPtr());
    } else {
      ValueInfo *VI;
      if (const GlobalAlias *GA = dyn_cast<GlobalAlias>(G)) {
        VI = analyzeGlobalAlias(GA);
      } else {
        assert(isa<GlobalVariable>(G) || isa<Function>(G));
        VI = analyzeGlobalRegion(G);
      }
      if (VI && !G->hasLocalLinkage()) {
        RH.handleRelation<DEPENDS_ON>(RH.D->ExternallyLinkableRegions.getPtr(),
            VI);
      }
      return VI;
    }
  }

  ValueInfo *analyzeGlobalAlias(const GlobalAlias *GA) {
    const Constant *Aliasee = GA->getAliasee();
    if (Aliasee) {
      // Even when not overridable, we have to make a new ValueInfo because
      // alias chains can have cycles.
      ValueInfo *VI = cacheNewValueInfo(GA);
      RH.handleRelation<DEPENDS_ON>(VI, analyzeValue(Aliasee));
      if (GA->mayBeOverridden()) {
        RH.handleRelation<DEPENDS_ON>(VI,
            RH.D->ExternallyLinkableRegions.getPtr());
      }
      return VI;
    } else if (GA->mayBeOverridden()) {
      // TODO: What does it mean for an alias to alias nothing?
      return cache(GA, RH.D->ExternallyLinkableRegions.getPtr());
    } else {
      // TODO: What does it mean for an alias to alias nothing?
      return cacheNil(GA);
    }
  }

  ValueInfo *analyzeGlobalRegion(const GlobalValue *G) {
    ValueInfo *RegionVI = getGlobalRegionInfo(G);
    ValueInfo *VI;
    if (G->mayBeOverridden()) {
      // It either points to this region or an externally-linkable region.
      VI = cacheNewValueInfo(G);
      RH.handleRelation<DEPENDS_ON>(VI, RegionVI);
      RH.handleRelation<DEPENDS_ON>(VI,
          RH.D->ExternallyLinkableRegions.getPtr());
    } else {
      // It can only point to this region.
      VI = cache(G, RegionVI);
    }
    if (const GlobalVariable *GV = dyn_cast<GlobalVariable>(G)) {
      assert(GV->hasInitializer());
      ValueInfo *InitializerValueInfo = analyzeValue(GV->getInitializer());
      if (InitializerValueInfo) {
        // Since Andersen's algorithm is flow-insensitive, the effect of an
        // initializer is the same as that of a store instruction, except that
        // it can only store to the definition of the symbol in this module.
        RH.handleRelation<STORED_TO>(InitializerValueInfo, RegionVI);
      }
    }
    return VI;
  }

  ValueInfo *analyzeArgument(const Argument *A) {
    ValueInfo *ArgumentValueInfo = cacheNewValueInfo(A);
    RH.handleRelation<ARGUMENT_FROM_CALLER>(ArgumentValueInfo,
        CurrentFunctionRegion);
    return ArgumentValueInfo;
  }

  ValueInfo *analyzeUser(const User *U) {
    typedef SmallVector<ValueInfo *, 3> ValueInfoVector;
    ValueInfoVector Set;
    for (User::const_op_iterator i = U->op_begin(), End = U->op_end(); i != End;
         ++i) {
      ValueInfo *VI = analyzeValue(*i);
      if (VI) {
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
      for (ValueInfoVector::const_iterator i = Set.begin(), End = Set.end();
           i != End; ++i) {
        RH.handleRelation<DEPENDS_ON>(Result, *i);
      }
      break;
    }
    return Result;
  }

  void processAtomicMutateInst(Instruction &I, Value *PointerOperand,
      Value *ValOperand) {
    ValueInfo *AddressAnalysis = analyzeValue(PointerOperand);
    ValueInfo *StoredValueInfo = analyzeValue(ValOperand);
    if (AddressAnalysis) {
      ValueInfo *LoadedValueInfo = cacheNewValueInfo(&I);
      processLoad(LoadedValueInfo, AddressAnalysis);
      processStore(StoredValueInfo, AddressAnalysis);
    } else {
      cacheNil(&I);
    }
  }

  void processLowLevelPointerIntrinsic(IntrinsicInst &I) {
    assert(I.getType()->isPointerTy());
    // We treat the result as a new region. This means that conflicting accesses
    // made through the result pointer may not be re-ordered relative to each
    // other, but they may be re-ordered with respect to accesses through other
    // pointers not based on the result. This is desirable because these
    // low-level intrinsics are for retrieving information about the generated
    // code, not the source-level code, so they should not be allowed to impede
    // optimizations. We also treat the instruction itself as not accessing any
    // memory, which allows optimizations to move it around or factor it out.
    // TODO: Figure out if this is safe.
    cacheNewRegion(&I);
  }
};

Data *InstructionAnalyzer::run(ModulePass *MP, Module &M) {
  return Visitor::run(MP, M);
}

}
}
