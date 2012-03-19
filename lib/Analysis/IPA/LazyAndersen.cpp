//===- LazyAndersen.cpp - Lazy Andersen's algorithm -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the LazyAndersen pass, which is a subset-based and
// context-, flow-, and field-insensitive points-to analysis based on a lazy
// version of Andersen's algorithm.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "lazy-andersen"
#include "llvm/Analysis/LazyAndersen.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/ilist_node.h"
//#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/Support/InstVisitor.h"
#include "llvm/User.h"
#include "llvm/Value.h"
#include <stdio.h>
using namespace llvm;

char LazyAndersen::ID = 0;
// TODO: What do these two bools mean?
INITIALIZE_PASS(LazyAndersen, "lazy-andersen",
                "Lazy Andersen's Algorithm for Points-To Analysis", false, true)

namespace {
  enum RelationDirection {
    INCOMING,
    OUTGOING
  };

  template<RelationDirection direction>
  class HalfRelation;
}

namespace llvm {
  // Specialize ilist_traits to use a ghostly sentinel.
  template<RelationDirection direction>
  struct ilist_traits<HalfRelation<direction> >
      : public ilist_default_traits<HalfRelation<direction> > {
    mutable ilist_half_node<HalfRelation<direction> > Sentinel;

  public:
    HalfRelation<direction> *createSentinel() const {
      return static_cast<HalfRelation<direction> *>(&Sentinel);
    }

    void destroySentinel(HalfRelation<direction> *) const {}

    HalfRelation<direction> *provideInitialHead() const {
      return createSentinel();
    }

    HalfRelation<direction> *ensureHead(HalfRelation<direction> *) const {
      return createSentinel();
    }

    static void noteHead(HalfRelation<direction> *, HalfRelation<direction> *) {
    }

    // Called by ilist destructor; do nothing.
    // TODO: Should we cast to Relation and delete it? Probably not, the destructor would
    // call remove(). in ilist.h, deleteNode() is called from erase() which already does
    // remove(), and calling remove() again will fail. But we could zero the VA field and
    // then delete so that it doesn't try to unlink again. Or we could unlink the other
    // direction and then delete and have the HalfRelation destructor be a no-op. There is
    // a removeFromList and addToList method on ilist_default_traits that we can override to
    // set/unset the List pointer.
    static void deleteNode(HalfRelation<direction> *) {}
  };
}

namespace {
  class ValueAnalysis :
      private ilist<HalfRelation<INCOMING> >,
      private ilist<HalfRelation<OUTGOING> > {
    template<RelationDirection direction>
    friend class HalfRelation;
    friend class Relation;
    const Value *V;

  public:
    ValueAnalysis(const Value *V) : V(V) {}
  };

  // A ValueAnalysis for an address that points to a single specific zone.
  class SingularValueAnalysis : public ValueAnalysis {
  public:
    SingularValueAnalysis(const Value *V) : ValueAnalysis(V) {}
  };

  ValueAnalysis *const EmptyValueAnalysis = 0;

  template<RelationDirection direction>
  class HalfRelation : private ilist_node<HalfRelation<direction> > {
    friend class ilist_nextprev_traits<HalfRelation>;
    friend class ilist_traits<HalfRelation>;

  protected:
    ilist<HalfRelation> *List;

    HalfRelation(ilist<HalfRelation> *List) : List(List) {
      assert(List && "List missing");
      link();
    }

    ~HalfRelation() {
      unlink();
    }

    void link() {
      List->push_back(this);
    }

    void unlink() {
      List->remove(typename ilist<HalfRelation>::iterator(this));
    }

    ValueAnalysis *getValueAnalysis() {
      return static_cast<ValueAnalysis *>(List);
    }
  };

  // A Relation is a directed edge in the analysis graph.
  class Relation :
      public HalfRelation<INCOMING>,
      public HalfRelation<OUTGOING> {
  public:
    Relation(ValueAnalysis *Src, ValueAnalysis *Dst)
      : HalfRelation<INCOMING>(Src), HalfRelation<OUTGOING>(Dst) {}

    virtual ~Relation() {}
  };

  class DependsOnRelation : public Relation {
  public:
    DependsOnRelation(ValueAnalysis *DependentValueAnalyis,
        ValueAnalysis *IndependentValueAnalysis)
      : Relation(DependentValueAnalyis, IndependentValueAnalysis) {}
  };

  class LoadedFromRelation : public Relation {
  public:
    LoadedFromRelation(ValueAnalysis *LoadedValueAnalyis,
        ValueAnalysis *AddressValueAnalysis)
      : Relation(LoadedValueAnalyis, AddressValueAnalysis) {}
  };

  class StoredToRelation : public Relation {
  public:
    StoredToRelation(ValueAnalysis *StoredValueAnalyis,
        ValueAnalysis *AddressValueAnalysis)
      : Relation(StoredValueAnalyis, AddressValueAnalysis) {}
  };

  class ReturnedFromCalleeRelation : public Relation {
  public:
    ReturnedFromCalleeRelation(ValueAnalysis *ReturnedValueAnalyis,
        ValueAnalysis *CalledValueAnalysis)
      : Relation(ReturnedValueAnalyis, CalledValueAnalysis) {}
  };

  class ArgumentToCalleeRelation : public Relation {
  public:
    ArgumentToCalleeRelation(ValueAnalysis *ArgumentValueAnalyis,
        ValueAnalysis *CalledValueAnalysis)
      : Relation(ArgumentValueAnalyis, CalledValueAnalysis) {}
  };

  class ArgumentFromCallerRelation : public Relation {
  public:
    ArgumentFromCallerRelation(ValueAnalysis *ArgumentValueAnalyis,
        ValueAnalysis *FunctionValueAnalysis)
      : Relation(ArgumentValueAnalyis, FunctionValueAnalysis) {}
  };

  class ReturnedToCallerRelation : public Relation {
  public:
    ReturnedToCallerRelation(ValueAnalysis *ReturnedValueAnalyis,
        ValueAnalysis *FunctionValueAnalysis)
      : Relation(ReturnedValueAnalyis, FunctionValueAnalysis) {}
  };
}

namespace llvm {
  class LazyAndersenData {
  public:
    // TODO: Should this be a ValueMap? Should the ValueAnalysis be refcounted?
    typedef DenseMap<const Value *, ValueAnalysis *> ValueToAnalysisMap;
    ValueToAnalysisMap ValueAnalyses;
  };
}

namespace {
  class FunctionVisitor : private InstVisitor<FunctionVisitor> {
    friend class InstVisitor<FunctionVisitor>;
    LazyAndersenData *Data;
    Function *CurrentFunction;

  public:
    FunctionVisitor(LazyAndersenData *Data) : Data(Data) {}

    void run(Function &F) {
      CurrentFunction = &F;
      // TODO: Handle var arg functions.
      visit(F);
    }

  private:
    void visitReturnInst(ReturnInst &I) {
      const Value *ReturnValue = I.getReturnValue();
      if (!ReturnValue) {
        return;
      }
      ValueAnalysis *ReturnedValueAnalysis = analyzeValue(ReturnValue);
      if (ReturnedValueAnalysis) {
        ValueAnalysis *FunctionValueAnalysis = analyzeValue(CurrentFunction);
        new ReturnedToCallerRelation(ReturnedValueAnalysis,
            FunctionValueAnalysis);
      }
    }

    void visitInvokeInst(InvokeInst &I) {
      visitCallOrInvokeInst(I);
    }

    void visitAllocaInst(AllocaInst &I) {
      cache(&I, new SingularValueAnalysis(&I));
    }

    void visitLoadInst(LoadInst &I) {
      ValueAnalysis *AddressAnalysis = analyzeValue(I.getPointerOperand());
      if (AddressAnalysis) {
        ValueAnalysis *LoadedValueAnalysis = cache(&I, new ValueAnalysis(&I));
        new LoadedFromRelation(LoadedValueAnalysis, AddressAnalysis);
      } else {
        cache(&I, EmptyValueAnalysis);
      }
      // TODO: Record that the current function may load this address.
    }

    void visitStoreInst(StoreInst &I) {
      ValueAnalysis *AddressAnalysis = analyzeValue(I.getPointerOperand());
      if (AddressAnalysis) {
        ValueAnalysis *StoredValueAnalysis = analyzeValue(I.getValueOperand());
        if (StoredValueAnalysis) {
          new StoredToRelation(StoredValueAnalysis, AddressAnalysis);
          // TODO: Record that the current function may store this address.
        }
      }
    }

    void visitAtomicCmpXchgInst(AtomicCmpXchgInst &I) {
      // TODO
    }

    void visitAtomicRMWInst(AtomicRMWInst &I) {
      // TODO
    }

    void visitPHINode(PHINode &I) {
      // TODO
    }

    void visitCallInst(CallInst &I) {
      visitCallOrInvokeInst(I);
    }

    void visitVAArgInst(VAArgInst &I) {
      // TODO
    }

    template<typename T>
    void visitCallOrInvokeInst(T &I) {
      const Value *CalledValue = I.getCalledValue();
      ValueAnalysis *CalledValueAnalysis = analyzeValue(CalledValue);
      if (CalledValueAnalysis) {
        for (unsigned i = 0; i < I.getNumArgOperands(); i++) {
          const Value *ArgumentValue = I.getArgOperand(i);
          ValueAnalysis *ArgumentValueAnalysis = analyzeValue(ArgumentValue);
          if (ArgumentValueAnalysis) {
            new ArgumentToCalleeRelation(ArgumentValueAnalysis,
                CalledValueAnalysis);
          }
        }
      }
      FunctionType *FunctionTy;
      Type *CalledTy = CalledValue->getType();
      if (!(FunctionTy = dyn_cast<FunctionType>(CalledTy))) {
        FunctionTy = cast<FunctionType>(cast<PointerType>(CalledTy)
            ->getPointerElementType());
      }
      if (!FunctionTy->getReturnType()->isVoidTy()) {
        ValueAnalysis *ReturnedValueAnalysis;
        if (CalledValueAnalysis) {
          ReturnedValueAnalysis = new ValueAnalysis(&I);
          new ReturnedFromCalleeRelation(ReturnedValueAnalysis,
              CalledValueAnalysis);
        } else {
          ReturnedValueAnalysis = EmptyValueAnalysis;
        }
        cache(&I, ReturnedValueAnalysis);
      }
      // TODO: Record that the current function may call this function.
    }

    void visitInstruction(Instruction &I) {
      assert(!I.mayReadOrWriteMemory() && "Unhandled memory instruction");
      cache(&I, analyzeUser(&I));
    }

    bool analyzed(const Value *V) {
      return Data->ValueAnalyses.count(V);
    }

    ValueAnalysis *cache(const Value *V, ValueAnalysis *VA) {
      assert(!analyzed(V));
      Data->ValueAnalyses[V] = VA;
      return VA;
    }

    ValueAnalysis *analyzeValue(const Value *V) {
      LazyAndersenData::ValueToAnalysisMap::const_iterator i =
          Data->ValueAnalyses.find(V);
      if (i != Data->ValueAnalyses.end()) {
        // Previously analyzed.
        return i->second;
      }
      // Else analyze now.
      assert(!isa<Instruction>(V) && "Instruction used before executed");
      ValueAnalysis *VA;
      if (const GlobalValue *G = dyn_cast<GlobalValue>(V)) {
        VA = analyzeGlobalValue(G);
      } else if (const Argument *A = dyn_cast<Argument>(V)) {
        VA = analyzeArgument(A);
      } else if (const User *U = dyn_cast<User>(V)) {
        VA = analyzeUser(U);
      } else {
        // TODO: Are there other types of Values that can point to things?
        VA = EmptyValueAnalysis;
      }
      return cache(V, VA);
    }

    ValueAnalysis *analyzeGlobalValue(const GlobalValue *G) {
      // TODO: Need to be aware of linkage here. Also, GlobalAlias may be
      // special. Also, a global might be initialized with a value that points
      // to something else.
      // TODO: Rather than a special class for this, we could create a regular
      // ValueAnalysis with its points-to set preconstructed.
      return new SingularValueAnalysis(G);
    }

    ValueAnalysis *analyzeArgument(const Argument *A) {
      ValueAnalysis *ArgumentValueAnalysis = new ValueAnalysis(A);
      ValueAnalysis *FunctionValueAnalysis = analyzeValue(CurrentFunction);
      new ArgumentFromCallerRelation(ArgumentValueAnalysis,
          FunctionValueAnalysis);
      return ArgumentValueAnalysis;
    }

    ValueAnalysis *analyzeUser(const User *U) {
      typedef SmallVector<ValueAnalysis *, 3> ValueAnalysisVector;
      ValueAnalysisVector Set;
      for (User::const_op_iterator i = U->op_begin(); i != U->op_end(); ++i) {
        ValueAnalysis *VA = analyzeValue(*i);
        if (VA != EmptyValueAnalysis) {
          Set.push_back(VA);
        }
      }
      ValueAnalysis *Result;
      switch (Set.size()) {
      case 0:
        Result = EmptyValueAnalysis;
        break;
      case 1:
        Result = Set.front();
        break;
      default:
        Result = new ValueAnalysis(U);
        for (ValueAnalysisVector::const_iterator i = Set.begin();
             i != Set.end(); ++i) {
          new DependsOnRelation(Result, *i);
        }
        break;
      }
      return Result;
    }
  };
}

LazyAndersen::LazyAndersen()
  : ModulePass(ID), Data(0) {
  initializeLazyAndersenPass(*PassRegistry::getPassRegistry());
}

bool LazyAndersen::runOnModule(Module &M) {
  if (!Data) {
    Data = new LazyAndersenData();
  }
  FunctionVisitor Visitor(Data);
  for (Module::iterator i = M.begin(); i != M.end(); ++i) {
    Visitor.run(*i);
  }
  // TODO
  printf("foo\n");
  return false;
}

void LazyAndersen::releaseMemory() {
  delete Data;
  Data = 0;
}

void LazyAndersen::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

void LazyAndersen::print(raw_ostream &OS, const Module *) const {
  // TODO
}
