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
#include "llvm/Support/InstVisitor.h"

namespace llvm {
  class LazyAndersenData;
}

namespace llvm {
namespace lazyandersen {
  class CallOrInvokeInstWrapperInterface;
  class ValueInfo;

  class InstructionAnalyzer : public InstVisitor<InstructionAnalyzer> {
    typedef SmallVector<std::pair<const PHINode *, ValueInfo *>, 3>
        PHINodeWorkVector;
    PHINodeWorkVector PHINodeWork;
    LazyAndersenData *Data;
    Function *CurrentFunction;

  public:
    static LazyAndersenData *run(Module &M);
    void visitReturnInst(ReturnInst &I);
    void visitInvokeInst(InvokeInst &I);
    void visitAllocaInst(AllocaInst &I);
    void visitLoadInst(LoadInst &I);
    void visitStoreInst(StoreInst &I);
    void visitAtomicCmpXchgInst(AtomicCmpXchgInst &I);
    void visitAtomicRMWInst(AtomicRMWInst &I);
    void visitPHINode(PHINode &I);
    void visitCallInst(CallInst &I);
    void visitVAArgInst(VAArgInst &I);
    void visitInstruction(Instruction &I);

  private:
    explicit InstructionAnalyzer(Module &M);
    void visitCallOrInvokeInst(Instruction &I,
        const CallOrInvokeInstWrapperInterface &W);
    void processPHINodes();
    bool analyzed(const Value *V);
    ValueInfo *cache(const Value *V, ValueInfo *VI);
    ValueInfo *createValueInfo(const Value *V);
    ValueInfo *createFinalizedValueInfo(const Value *V);
    ValueInfo *analyzeValue(const Value *V);
    ValueInfo *analyzeGlobalValue(const GlobalValue *G);
    ValueInfo *analyzeArgument(const Argument *A);
    ValueInfo *analyzeUser(const User *U);
  };
}
}

#endif
