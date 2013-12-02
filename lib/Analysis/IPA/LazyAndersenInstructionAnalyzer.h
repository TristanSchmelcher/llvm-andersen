//===- LazyAndersenInstructionAnalyzer.h - instruction analysis -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for instruction analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENINSTRUCTIONANALYZER_H
#define LAZYANDERSENINSTRUCTIONANALYZER_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/InstVisitor.h"

namespace llvm {
  class ModulePass;
}

namespace llvm {
namespace lazyandersen {
  class LazyAndersenData;
  class ValueInfo;

  class InstructionAnalyzer : public InstVisitor<InstructionAnalyzer> {
    typedef SmallVector<std::pair<const PHINode *, ValueInfo *>, 3>
        PHINodeWorkVector;
    ModulePass *MP;
    PHINodeWorkVector PHINodeWork;
    LazyAndersenData *Data;
    Function *CurrentFunction;

  public:
    static LazyAndersenData *run(ModulePass *MP, Module &M);
    void visitReturnInst(ReturnInst &I);
    void visitAllocaInst(AllocaInst &I);
    void visitLoadInst(LoadInst &I);
    void visitStoreInst(StoreInst &I);
    void visitAtomicCmpXchgInst(AtomicCmpXchgInst &I);
    void visitAtomicRMWInst(AtomicRMWInst &I);
    void visitPHINode(PHINode &I);
    void visitCallSite(CallSite CS);
    void visitVAStartInst(VAStartInst &I);
    void visitVAEndInst(VAEndInst &I);
    void visitVACopyInst(VACopyInst &I);
    void visitVAArgInst(VAArgInst &I);
    void visitInstruction(Instruction &I);
    void visitFenceInst(FenceInst &I);
    void visitMemSetInst(MemSetInst &I);
    void visitMemCpyInst(MemCpyInst &I);
    void visitMemMoveInst(MemMoveInst &I);

  private:
    InstructionAnalyzer(ModulePass *MP, Module &M);
    static LazyAndersenData *createLazyAndersenData();
    void processFunction(Function &F);
    bool analyzed(const Value *V);
    static ValueInfo *makeRegion(ValueInfo *VI);
    static ValueInfo *createValueInfo(const Value *V);
    static ValueInfo *createRegion(const Value *V);
    ValueInfo *cache(const Value *V, ValueInfo *VI);
    ValueInfo *cacheNewValueInfo(const Value *V);
    ValueInfo *cacheNewRegion(const Value *V);
    ValueInfo *cacheNil(const Value *V);
    ValueInfo *getGlobalRegionInfo(const GlobalValue *G);
    ValueInfo *createAnonymousValueInfo();
    ValueInfo *analyzeValue(const Value *V);
    ValueInfo *analyzeGlobalValue(const GlobalValue *G);
    ValueInfo *analyzeGlobalAlias(const GlobalAlias *GA);
    ValueInfo *analyzeGlobalRegion(const GlobalValue *G);
    ValueInfo *analyzeArgument(const Argument *A);
    ValueInfo *analyzeUser(const User *U);
    void visitMemTransferInst(MemTransferInst &I);
  };
}
}

#endif
