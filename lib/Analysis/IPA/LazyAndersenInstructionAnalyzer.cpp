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

namespace llvm {
namespace lazyandersen {
// Wrapper for the methods of CallInst and InvokeInst that ought to be
// inherited from a common interface but aren't. :/
class CallOrInvokeInstWrapperInterface {
public:
  virtual const Value *getCalledValue() const = 0;
  virtual unsigned getNumArgOperands() const = 0;
  virtual const Value *getArgOperand(unsigned i) const = 0;
};
}
}

namespace {
  template<typename T>
  class CallOrInvokeInstWrapper : public CallOrInvokeInstWrapperInterface {
    const T &I;

  public:
    explicit CallOrInvokeInstWrapper(const T &I);
    virtual const Value *getCalledValue() const;
    virtual unsigned getNumArgOperands() const;
    virtual const Value *getArgOperand(unsigned i) const;
  };

  // class CallOrInvokeInstWrapper
  template<typename T>
  inline CallOrInvokeInstWrapper<T>::CallOrInvokeInstWrapper(const T &I)
    : I(I) {}

  template<typename T>
  const Value *CallOrInvokeInstWrapper<T>::getCalledValue() const {
    return I.getCalledValue();
  }

  template<typename T>
  unsigned CallOrInvokeInstWrapper<T>::getNumArgOperands() const {
    return I.getNumArgOperands();
  }

  template<typename T>
  const Value *CallOrInvokeInstWrapper<T>::getArgOperand(unsigned i) const {
    return I.getArgOperand(i);
  }
}

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
    ValueInfo *FunctionValueInfo = analyzeValue(CurrentFunction);
    RelationHandler::handleRelation<RETURNED_TO_CALLER>(ReturnedValueInfo,
        FunctionValueInfo);
  }
}

void InstructionAnalyzer::visitInvokeInst(InvokeInst &I) {
  visitCallOrInvokeInst(I, CallOrInvokeInstWrapper<InvokeInst>(I));
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

void InstructionAnalyzer::visitCallInst(CallInst &I) {
  visitCallOrInvokeInst(I, CallOrInvokeInstWrapper<CallInst>(I));
}

void InstructionAnalyzer::visitVAArgInst(VAArgInst &I) {
  // TODO: Not right. Needs special handling for va_start intrinsic.
  cacheNil(&I);
}

void InstructionAnalyzer::visitInstruction(Instruction &I) {
  assert(!I.mayReadOrWriteMemory() && "Unhandled memory instruction");
  analyzeUser(&I);
}

void InstructionAnalyzer::visitFenceInst(FenceInst &I) {
  // No-op (can't use default visitInstruction because it fails the assert).
}

InstructionAnalyzer::InstructionAnalyzer(ModulePass *MP, Module &M)
  : MP(MP), Data(createLazyAndersenData()) {
  for (Module::iterator i = M.begin(); i != M.end(); ++i) {
    Function &F(*i);
    if (!F.isDeclaration()) {
      processFunction(F);
    }
  }
}

LazyAndersenData *InstructionAnalyzer::createLazyAndersenData() {
  // Placeholder for all externally-defined regions.
  ValueInfo *ExternallyDefinedRegions = createRegion(0);

  // All global regions that are externally accessible by way of linkage. This
  // is the set of all internally-defined global regions with external linkage
  // plus a placeholder for externally-defined (global) regions.
  ValueInfo *ExternallyLinkableRegions = createValueInfo(0);
  // Externally-defined non-global regions are indistinguishable from
  // externally-defined global regions, so we can use ExternallyDefinedRegions
  // here.
  RelationHandler::handleRelation<DEPENDS_ON>(ExternallyLinkableRegions,
      ExternallyDefinedRegions);

  // All regions that are externally accessible in any manner. This is the set
  // of all externally-defined regions, all link-accessible global regions, and
  // all internally-defined regions that can be accessed by dereferencing or
  // calling them.
  ValueInfo *ExternallyAccessibleRegions = createValueInfo(0);
  // This includes ExternallyDefinedRegions.
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

  return new LazyAndersenData(ExternallyDefinedRegions,
                              ExternallyLinkableRegions,
                              ExternallyAccessibleRegions);
}

void InstructionAnalyzer::processFunction(Function &F) {
  CurrentFunction = &F;
  // Visit the basic blocks in a depth-first traversal of the dominator tree.
  // This ensures that we visit each instruction before each non-PHI use of it.
  // TODO: This skips (seemingly) unreachable code, so a future alias lookup on
  // such code may break.
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

void InstructionAnalyzer::visitCallOrInvokeInst(Instruction &I,
    const CallOrInvokeInstWrapperInterface &W) {
  const Value *CalledValue = W.getCalledValue();
  ValueInfo *CalledValueInfo = analyzeValue(CalledValue);
  if (CalledValueInfo) {
    for (unsigned i = 0; i < W.getNumArgOperands(); i++) {
      const Value *ArgumentValue = W.getArgOperand(i);
      ValueInfo *ArgumentValueInfo = analyzeValue(ArgumentValue);
      if (ArgumentValueInfo) {
        RelationHandler::handleRelation<ARGUMENT_TO_CALLEE>(ArgumentValueInfo,
            CalledValueInfo);
      }
    }
  }
  if (!I.getType()->isVoidTy()) {
    if (CalledValueInfo) {
      ValueInfo *ReturnedValueInfo = cacheNewValueInfo(&I);
      RelationHandler::handleRelation<RETURNED_FROM_CALLEE>(ReturnedValueInfo,
          CalledValueInfo);
    } else {
      cacheNil(&I);
    }
  }
  // TODO: Record that the current function may call this function.
}

bool InstructionAnalyzer::analyzed(const Value *V) {
  return Data->ValueInfos.count(V);
}

ValueInfo *InstructionAnalyzer::createValueInfo(const Value *V) {
  return new ValueInfo(V);
}

ValueInfo *InstructionAnalyzer::createRegion(const Value *V) {
  ValueInfo *VI = createValueInfo(V);
  VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()
      ->addValueInfo(VI);
  return VI;
}

ValueInfo *InstructionAnalyzer::cache(const Value *V, ValueInfo *VI) {
  assert(!analyzed(V));
  Data->ValueInfos[V] = VI;
  return VI;
}

ValueInfo *InstructionAnalyzer::cacheNewValueInfo(const Value *V) {
  return cache(V, createValueInfo(V));
}

ValueInfo *InstructionAnalyzer::cacheNewRegion(const Value *V) {
  return cache(V, createRegion(V));
}

ValueInfo *InstructionAnalyzer::cacheNil(const Value *V) {
  return cache(V, ValueInfo::Nil);
}

ValueInfo *InstructionAnalyzer::analyzeValue(const Value *V) {
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
  } else if (const User *U = dyn_cast<User>(V)) {
    VI = analyzeUser(U);
  } else {
#ifndef NDEBUG
    if (const Instruction *I = dyn_cast<Instruction>(V)) {
      // Since we visit BBs in control-flow order, this can only happen if a
      // reachable BB has a PHI node with an incoming value from an unreachable
      // BB. Since the instruction cannot possibly execute, we can pretend that
      // its result points to nothing.
      assert(!MP->getAnalysis<DominatorTree>(*CurrentFunction)
                 .isReachableFromEntry(I->getParent()) &&
             "Instruction used before executed");
    }
#endif
    // TODO: Are there other types of Values that can point to things?
    VI = cacheNil(V);
  }
  return VI;
}

namespace {

// Whether or not the linkage of this symbol makes it accessible from other
// modules. For functions, this means directly callable; for global variables,
// this means directly read/writable.
bool hasExternallyAccessibleLinkage(const GlobalValue *G) {
  // TODO: Figure out the precise set of linkage types.
  return G->hasExternalLinkage();
}

}

ValueInfo *InstructionAnalyzer::analyzeGlobalValue(const GlobalValue *G) {
  // TODO: Need to handle weak linkage too.
  bool external = hasExternallyAccessibleLinkage(G);
  if (G->isDeclaration()) {
    if (external) {
      return cache(G, Data->ExternallyLinkableRegions.getPtr());
    } else {
      // Use of undefined static-linkage symbols is not legal. Pretend it
      // points to nothing.
      return cacheNil(G);
    }
  } else {
    ValueInfo *GlobalValueInfo;
    if (const GlobalAlias *GA = dyn_cast<GlobalAlias>(G)) {
      const GlobalValue *Aliasee = GA->getAliasedGlobal();
      if (Aliasee) {
        GlobalValueInfo = cacheNewValueInfo(G);
        RelationHandler::handleRelation<DEPENDS_ON>(
            GlobalValueInfo, analyzeGlobalValue(Aliasee));
      } else {
        // TODO: What does it mean for an alias to alias nothing?
        return cacheNil(G);
      }
    } else {
      // Either GlobalVariable or Function.
      GlobalValueInfo = cacheNewRegion(G);
      if (const GlobalVariable *GV = dyn_cast<GlobalVariable>(G)) {
        ValueInfo *InitializerValueInfo = analyzeValue(GV->getInitializer());
        if (InitializerValueInfo) {
          // Since Andersen's algorithm is flow-insensitive, the effect of an
          // initializer is the same as that of a store instruction.
          RelationHandler::handleRelation<STORED_TO>(InitializerValueInfo,
              GlobalValueInfo);
        }
      }
    }
    if (external) {
      RelationHandler::handleRelation<DEPENDS_ON>(
          Data->ExternallyLinkableRegions.getPtr(), GlobalValueInfo);
    }
    return GlobalValueInfo;
  }
}

ValueInfo *InstructionAnalyzer::analyzeArgument(const Argument *A) {
  ValueInfo *ArgumentValueInfo = cacheNewValueInfo(A);
  ValueInfo *FunctionValueInfo = analyzeValue(CurrentFunction);
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
