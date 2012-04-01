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
#include "llvm/ADT/IntrusiveRefCntPtr.h"
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
  class HalfRelationBase;
}

namespace llvm {
  template<>
  struct ilist_traits<HalfRelationBase>
      : public ilist_default_traits<HalfRelationBase> {
    mutable ilist_half_node<HalfRelationBase> Sentinel;

  public:
    HalfRelationBase *createSentinel() const;

    void destroySentinel(HalfRelationBase *) const {}

    HalfRelationBase *provideInitialHead() const {
      return createSentinel();
    }

    HalfRelationBase *ensureHead(HalfRelationBase *) const {
      return createSentinel();
    }

    static void noteHead(HalfRelationBase *, HalfRelationBase *) {}

    void addNodeToList(HalfRelationBase *node);

    void removeNodeFromList(HalfRelationBase *node);

    void transferNodesFromList(
        ilist_node_traits<HalfRelationBase> &that,
        ilist_iterator<HalfRelationBase> first,
        ilist_iterator<HalfRelationBase> last);
  };
}

namespace {
  typedef ilist<HalfRelationBase> HalfRelationBaseList;

  class HalfRelationBase : private ilist_node<HalfRelationBase> {
    friend struct ilist_nextprev_traits<HalfRelationBase>;
    friend struct ilist_node_traits<HalfRelationBase>;
    friend struct ilist_traits<HalfRelationBase>;

  protected:
    HalfRelationBaseList *List;

    HalfRelationBase(HalfRelationBaseList *InitialList) : List(0) {
      assert(InitialList);
      InitialList->push_back(this);
      assert(List == InitialList);
    }

    // virtual so that deleteNode in ilist_node_traits will delete the whole
    // Relation object.
    virtual ~HalfRelationBase() {
      if (List) {
        List->remove(HalfRelationBaseList::iterator(this));
        assert(!List);
      }
    }

  private:
    void added(HalfRelationBaseList *ToList) {
      assert(!List);
      List = ToList;
    }

    void removed(HalfRelationBaseList *FromList) {
      assert(List == FromList);
      List = 0;
    }

    void transferred(HalfRelationBaseList *ToList,
        HalfRelationBaseList *FromList) {
      assert(List == FromList);
      List = ToList;
    }
  };
}

namespace llvm {
  inline HalfRelationBase *ilist_traits<HalfRelationBase>::createSentinel()
      const {
    return static_cast<HalfRelationBase *>(&Sentinel);
  }

  inline void ilist_traits<HalfRelationBase>::addNodeToList(
      HalfRelationBase *node) {
    node->added(static_cast<HalfRelationBaseList *>(this));
  }

  inline void ilist_traits<HalfRelationBase>::removeNodeFromList(
      HalfRelationBase *node) {
    node->removed(static_cast<HalfRelationBaseList *>(this));
  }

  inline void ilist_traits<HalfRelationBase>::transferNodesFromList(
      ilist_node_traits<HalfRelationBase> &that,
      ilist_iterator<HalfRelationBase> first,
      ilist_iterator<HalfRelationBase> last) {
    for (ilist_iterator<HalfRelationBase> i = first; i != last; i++) {
      i->transferred(static_cast<HalfRelationBaseList *>(this),
          static_cast<HalfRelationBaseList *>(&that));
    }
  }
}

namespace {
  enum RelationDirection {
    INCOMING,
    OUTGOING
  };

  template<RelationDirection direction>
  class HalfRelationList : public HalfRelationBaseList {
  };

  class ValueAnalysis :
      private RefCountedBase<ValueAnalysis>,
      private HalfRelationList<INCOMING>,
      private HalfRelationList<OUTGOING> {
    template<RelationDirection direction> friend class HalfRelation;
    friend struct IntrusiveRefCntPtrInfo<ValueAnalysis>;
    friend class RefCountedBase<ValueAnalysis>;
    const Value *V;

  public:
    ValueAnalysis(const Value *V) : V(V) {}

  protected:
    ~ValueAnalysis() {}
  };

  typedef IntrusiveRefCntPtr<ValueAnalysis> ValueAnalysisRef;

  // A ValueAnalysis for an address that points to a single specific zone.
  // TODO: Replace with a static method to create a base ValueAnalysis with its
  // points-to set precreated.
  class SingularValueAnalysis : public ValueAnalysis {
  public:
    SingularValueAnalysis(const Value *V) : ValueAnalysis(V) {}
  };

  ValueAnalysis *const EmptyValueAnalysis = 0;

  template<RelationDirection direction>
  class HalfRelation : public HalfRelationBase {
  public:
    HalfRelation(ValueAnalysis *VA)
      : HalfRelationBase(static_cast<HalfRelationList<direction> *>(VA)) {}

    ValueAnalysis *getValueAnalysis() const {
      assert(List);
      return static_cast<ValueAnalysis *>(
          static_cast<HalfRelationList<direction> *>(List));
    }

  protected:
    ~HalfRelation() {}
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
    // TODO: Should this be a ValueMap?
    typedef DenseMap<const Value *, ValueAnalysisRef> ValueToAnalysisMap;
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
        return i->second.getPtr();
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
