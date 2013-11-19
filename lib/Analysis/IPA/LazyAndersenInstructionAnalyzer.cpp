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
  cache(&I, createFinalizedValueInfo(&I));
}

void InstructionAnalyzer::visitLoadInst(LoadInst &I) {
  ValueInfo *AddressAnalysis = analyzeValue(I.getPointerOperand());
  if (AddressAnalysis) {
    ValueInfo *LoadedValueInfo = cache(&I, createValueInfo(&I));
    RelationHandler::handleRelation<LOADED_FROM>(LoadedValueInfo,
        AddressAnalysis);
  } else {
    cache(&I, ValueInfo::Nil);
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
    ValueInfo *LoadedValueInfo = cache(&I, createValueInfo(&I));
    RelationHandler::handleRelation<LOADED_FROM>(LoadedValueInfo,
        AddressAnalysis);
    ValueInfo *StoredValueInfo = analyzeValue(I.getNewValOperand());
    if (StoredValueInfo) {
      RelationHandler::handleRelation<STORED_TO>(StoredValueInfo,
          AddressAnalysis);
    }
  } else {
    cache(&I, ValueInfo::Nil);
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
    ValueInfo *LoadedValueInfo = cache(&I, createValueInfo(&I));
    RelationHandler::handleRelation<LOADED_FROM>(LoadedValueInfo,
        AddressAnalysis);
    ValueInfo *StoredValueInfo = analyzeValue(I.getValOperand());
    if (StoredValueInfo) {
      RelationHandler::handleRelation<STORED_TO>(StoredValueInfo,
          AddressAnalysis);
    }
  } else {
    cache(&I, ValueInfo::Nil);
  }
}

void InstructionAnalyzer::visitPHINode(PHINode &I) {
  ValueInfo *PHIAnalysis = cache(&I, createValueInfo(&I));
  PHINodeWork.push_back(PHINodeWorkVector::value_type(&I, PHIAnalysis));
}

void InstructionAnalyzer::visitCallInst(CallInst &I) {
  visitCallOrInvokeInst(I, CallOrInvokeInstWrapper<CallInst>(I));
}

void InstructionAnalyzer::visitVAArgInst(VAArgInst &I) {
  // TODO: Not right. Needs special handling for va_start intrinsic.
  cache(&I, ValueInfo::Nil);
}

void InstructionAnalyzer::visitInstruction(Instruction &I) {
  assert(!I.mayReadOrWriteMemory() && "Unhandled memory instruction");
  cache(&I, analyzeUser(&I));
}

void InstructionAnalyzer::visitFenceInst(FenceInst &I) {
  // No-op (can't use default visitInstruction because it fails the assert).
}

InstructionAnalyzer::InstructionAnalyzer(ModulePass *MP, Module &M)
  : MP(MP), Data(new LazyAndersenData()) {
  for (Module::iterator i = M.begin(); i != M.end(); ++i) {
    Function &F(*i);
    if (!F.isDeclaration()) {
      processFunction(F);
    }
  }
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
    ValueInfo *ReturnedValueInfo;
    if (CalledValueInfo) {
      ReturnedValueInfo = createValueInfo(&I);
      RelationHandler::handleRelation<RETURNED_FROM_CALLEE>(ReturnedValueInfo,
          CalledValueInfo);
    } else {
      ReturnedValueInfo = ValueInfo::Nil;
    }
    cache(&I, ReturnedValueInfo);
  }
  // TODO: Record that the current function may call this function.
}

bool InstructionAnalyzer::analyzed(const Value *V) {
  return Data->ValueInfos.count(V);
}

ValueInfo *InstructionAnalyzer::cache(const Value *V, ValueInfo *VI) {
  assert(!analyzed(V));
  Data->ValueInfos[V] = VI;
  return VI;
}

ValueInfo *InstructionAnalyzer::createValueInfo(const Value *V) {
  return new ValueInfo(V);
}

ValueInfo *InstructionAnalyzer::createFinalizedValueInfo(const Value *V) {
  ValueInfo *VI = createValueInfo(V);
  VI->getAlgorithmResult<PointsToAlgorithm, INSTRUCTION_ANALYSIS_PHASE>()
      ->addValueInfo(VI);
  return VI;
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
#ifndef NDEBUG
  } else if (const Instruction *I = dyn_cast<Instruction>(V)) {
    // Since we visit BBs in control-flow order, this can only happen if a
    // reachable BB has a PHI node with an incoming value from an unreachable
    // BB. Since the instruction cannot possibly execute, we can pretend that
    // its result points to nothing.
    assert(!MP->getAnalysis<DominatorTree>(*CurrentFunction)
               .isReachableFromEntry(I->getParent()) &&
           "Instruction used before executed");
    VI = ValueInfo::Nil;
#endif
  } else {
    // TODO: Are there other types of Values that can point to things?
    VI = ValueInfo::Nil;
  }
  return cache(V, VI);
}

ValueInfo *InstructionAnalyzer::analyzeGlobalValue(const GlobalValue *G) {
  // TODO: Need to be aware of linkage here. Also, GlobalAlias may be
  // special.
  ValueInfo *GlobalValueInfo = createFinalizedValueInfo(G);
  if (const GlobalVariable *GV = dyn_cast<GlobalVariable>(G)) {
    if (GV->hasInitializer()) {
      ValueInfo *InitializerValueInfo = analyzeValue(GV->getInitializer());
      if (InitializerValueInfo) {
        // Since Andersen's algorithm is flow-insensitive, the effect of an
        // initializer is the same as that of a store instruction.
        RelationHandler::handleRelation<STORED_TO>(InitializerValueInfo,
            GlobalValueInfo);
      }
    }
  }
  return GlobalValueInfo;
}

ValueInfo *InstructionAnalyzer::analyzeArgument(const Argument *A) {
  ValueInfo *ArgumentValueInfo = createValueInfo(A);
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
    Result = ValueInfo::Nil;
    break;
  case 1:
    Result = Set.front();
    break;
  default:
    Result = createValueInfo(U);
    for (ValueInfoVector::const_iterator i = Set.begin(); i != Set.end(); ++i) {
      RelationHandler::handleRelation<DEPENDS_ON>(Result, *i);
    }
    break;
  }
  return Result;
}
