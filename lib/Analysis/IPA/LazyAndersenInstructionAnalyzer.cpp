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
#include "LazyAndersenAnalysisResultCacheEntry.h"
#include "LazyAndersenArgumentFromCallerRelation.h"
#include "LazyAndersenArgumentToCalleeRelation.h"
#include "LazyAndersenData.h"
#include "LazyAndersenDependsOnRelation.h"
#include "LazyAndersenLoadedFromRelation.h"
#include "LazyAndersenReturnedFromCalleeRelation.h"
#include "LazyAndersenReturnedToCallerRelation.h"
#include "LazyAndersenStoredToRelation.h"
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
    new ReturnedToCallerRelation(ReturnedValueInfo,
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
    new LoadedFromRelation(LoadedValueInfo, AddressAnalysis);
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
      new StoredToRelation(StoredValueInfo, AddressAnalysis);
      // TODO: Record that the current function may store this address.
    }
  }
}

void InstructionAnalyzer::visitAtomicCmpXchgInst(AtomicCmpXchgInst &I) {
  // TODO
}

void InstructionAnalyzer::visitAtomicRMWInst(AtomicRMWInst &I) {
  // TODO
}

void InstructionAnalyzer::visitPHINode(PHINode &I) {
  ValueInfo *PHIAnalysis = cache(&I, createValueInfo(&I));
  PHINodeWork.push_back(PHINodeWorkVector::value_type(&I, PHIAnalysis));
}

void InstructionAnalyzer::visitCallInst(CallInst &I) {
  visitCallOrInvokeInst(I, CallOrInvokeInstWrapper<CallInst>(I));
}

void InstructionAnalyzer::visitVAArgInst(VAArgInst &I) {
  // TODO
}

void InstructionAnalyzer::visitInstruction(Instruction &I) {
  assert(!I.mayReadOrWriteMemory() && "Unhandled memory instruction");
  cache(&I, analyzeUser(&I));
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
  // such code may break. We should depend on a transform pass that removes
  // unreachable code.
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
        new DependsOnRelation(PHIAnalysis, OperandAnalysis);
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
        new ArgumentToCalleeRelation(ArgumentValueInfo,
            CalledValueInfo);
      }
    }
  }
  if (!I.getType()->isVoidTy()) {
    ValueInfo *ReturnedValueInfo;
    if (CalledValueInfo) {
      ReturnedValueInfo = createValueInfo(&I);
      new ReturnedFromCalleeRelation(ReturnedValueInfo,
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
  return new ValueInfo(V, &Data->ValueInfos);
}

ValueInfo *InstructionAnalyzer::createFinalizedValueInfo(const Value *V) {
  ValueInfo *VI = createValueInfo(V);
  AnalysisResult *AR = new AnalysisResult();
  AR->addWork(new AnalysisResultValueInfoEntry(VI));
  VI->setAlgorithmResultSpecialCase(POINTS_TO_SET, AR);
  return VI;
}

ValueInfo *InstructionAnalyzer::analyzeValue(const Value *V) {
  ValueInfo::Map::const_iterator i = Data->ValueInfos.find(V);
  if (i != Data->ValueInfos.end()) {
    // Previously analyzed.
    return i->second.getPtr();
  }
  // Else analyze now.
  assert(!isa<Instruction>(V) && "Instruction used before executed");
  ValueInfo *VI;
  if (const GlobalValue *G = dyn_cast<GlobalValue>(V)) {
    VI = analyzeGlobalValue(G);
  } else if (const Argument *A = dyn_cast<Argument>(V)) {
    VI = analyzeArgument(A);
  } else if (const User *U = dyn_cast<User>(V)) {
    VI = analyzeUser(U);
  } else {
    // TODO: Are there other types of Values that can point to things?
    VI = ValueInfo::Nil;
  }
  return cache(V, VI);
}

ValueInfo *InstructionAnalyzer::analyzeGlobalValue(const GlobalValue *G) {
  // TODO: Need to be aware of linkage here. Also, GlobalAlias may be
  // special. Also, a global might be initialized with a value that points
  // to something else, in which case we need to pre-create the
  // CONTENT_POINTS_TO_SET result.
  return createFinalizedValueInfo(G);
}

ValueInfo *InstructionAnalyzer::analyzeArgument(const Argument *A) {
  ValueInfo *ArgumentValueInfo = createValueInfo(A);
  ValueInfo *FunctionValueInfo = analyzeValue(CurrentFunction);
  new ArgumentFromCallerRelation(ArgumentValueInfo,
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
    for (ValueInfoVector::const_iterator i = Set.begin();
         i != Set.end(); ++i) {
      new DependsOnRelation(Result, *i);
    }
    break;
  }
  return Result;
}
