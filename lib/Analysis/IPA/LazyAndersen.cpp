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
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/InstVisitor.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/User.h"
#include "llvm/Value.h"
#include <sstream>

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
      : ilist_default_traits<HalfRelationBase> {
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

    void addNodeToList(HalfRelationBase *Node);

    void removeNodeFromList(HalfRelationBase *Node);

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
    // Relation object properly.
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

inline HalfRelationBase *ilist_traits<HalfRelationBase>::createSentinel()
    const {
  return static_cast<HalfRelationBase *>(&Sentinel);
}

inline void ilist_traits<HalfRelationBase>::addNodeToList(
    HalfRelationBase *Node) {
  Node->added(static_cast<HalfRelationBaseList *>(this));
}

inline void ilist_traits<HalfRelationBase>::removeNodeFromList(
    HalfRelationBase *Node) {
  Node->removed(static_cast<HalfRelationBaseList *>(this));
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

namespace {
  enum RelationDirection {
    INCOMING,
    OUTGOING
  };

  template<RelationDirection direction>
  struct direction_traits;

  template<>
  struct direction_traits<INCOMING> {
    static const RelationDirection OppositeDirection = OUTGOING;
  };

  template<>
  struct direction_traits<OUTGOING> {
    static const RelationDirection OppositeDirection = INCOMING;
  };

  template<RelationDirection direction>
  class HalfRelationList : public HalfRelationBaseList {};

  class ValueAnalysis;
  typedef IntrusiveRefCntPtr<ValueAnalysis> ValueAnalysisRef;
  // TODO: Should this be a ValueMap?
  typedef DenseMap<const Value *, ValueAnalysisRef> ValueToAnalysisMap;

  class ValueAnalysis :
      private RefCountedBase<ValueAnalysis>,
      public HalfRelationList<INCOMING>,
      public HalfRelationList<OUTGOING> {
    friend struct IntrusiveRefCntPtrInfo<ValueAnalysis>;
    friend class RefCountedBase<ValueAnalysis>;
    // The Value that maps to this object. (If this analysis applies to multiple
    // Values, this is the first one that was analyzed.)
    const Value *V;
    // The map that this analysis is in.
    ValueToAnalysisMap *Map;

  public:
    ValueAnalysis(const Value *V, ValueToAnalysisMap *Map) : V(V), Map(Map) {}

    const Value *getValue() const { return V; }
    ValueToAnalysisMap *getMap() const { return Map; }

  protected:
    ~ValueAnalysis() {}
  };

  ValueAnalysis *const EmptyValueAnalysis = 0;

  class Relation;

  template<RelationDirection direction>
  class HalfRelation : public HalfRelationBase {
  public:
    typedef HalfRelation<direction_traits<direction>::OppositeDirection>
        OppositeHalfRelationTy;

    HalfRelation(ValueAnalysis *VA)
      : HalfRelationBase(static_cast<HalfRelationList<direction> *>(VA)) {}

    ValueAnalysis *getValueAnalysis() const {
      assert(List);
      return static_cast<ValueAnalysis *>(
          static_cast<HalfRelationList<direction> *>(List));
    }

    OppositeHalfRelationTy *getOtherHalf() {
      return static_cast<OppositeHalfRelationTy *>(
          static_cast<Relation *>(this));
    }

    const OppositeHalfRelationTy *getOtherHalf() const {
      return static_cast<const OppositeHalfRelationTy *>(
          static_cast<const Relation *>(this));
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
      cache(&I, createFinalizedValueAnalysis(&I));
    }

    void visitLoadInst(LoadInst &I) {
      ValueAnalysis *AddressAnalysis = analyzeValue(I.getPointerOperand());
      if (AddressAnalysis) {
        ValueAnalysis *LoadedValueAnalysis = cache(&I, createValueAnalysis(&I));
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
          ReturnedValueAnalysis = createValueAnalysis(&I);
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

    ValueAnalysis *createValueAnalysis(const Value *V) {
      return new ValueAnalysis(V, &Data->ValueAnalyses);
    }

    ValueAnalysis *createFinalizedValueAnalysis(const Value *V) {
      // TODO: Pass finalized points-to set as argument and pre-create the set.
      return createValueAnalysis(V);
    }

    ValueAnalysis *analyzeValue(const Value *V) {
      ValueToAnalysisMap::const_iterator i = Data->ValueAnalyses.find(V);
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
      return createFinalizedValueAnalysis(G);
    }

    ValueAnalysis *analyzeArgument(const Argument *A) {
      ValueAnalysis *ArgumentValueAnalysis = createValueAnalysis(A);
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
        Result = createValueAnalysis(U);
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
  assert(!Data);
  Data = new LazyAndersenData();
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

// Iterators used exclusively for the graph traits classes below.
namespace {
  template<typename OriginalIteratorTy, typename ValueTy,
      typename AdapterFunctorTy>
  class forward_iterator_adapter {
    OriginalIteratorTy i;
    AdapterFunctorTy AdapterFunctor;

  public:
    typedef typename OriginalIteratorTy::difference_type difference_type;
    typedef ValueTy value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef std::forward_iterator_tag iterator_category;

    forward_iterator_adapter() {}
    forward_iterator_adapter(const OriginalIteratorTy &i) : i(i) {}

    value_type operator*() const {
      return AdapterFunctor(*i);
    }

    bool operator==(const forward_iterator_adapter &that) const {
      return i == that.i;
    }

    bool operator!=(const forward_iterator_adapter &that) const {
      return i != that.i;
    }

    forward_iterator_adapter &operator++() {
      ++i;
      return *this;
    }

    forward_iterator_adapter operator++(int) {
      forward_iterator_adapter tmp = *this; ++*this; return tmp;
    }
  };

  class ValueToAnalysisMapIteratorAdapter {
    struct AdapterFunctor {
      const ValueToAnalysisMap::value_type *operator()(
          const ValueToAnalysisMap::value_type &value) const {
        return &value;
      }
    };

  public:
    typedef forward_iterator_adapter<ValueToAnalysisMap::const_iterator,
        const ValueToAnalysisMap::value_type *, AdapterFunctor> iterator;
  };

  class OutgoingHalfRelationIteratorAdapter {
    struct AdapterFunctor {
      ValueToAnalysisMap::value_type *operator()(const HalfRelationBase &HR) const {
        ValueAnalysis *VA = static_cast<const HalfRelation<OUTGOING> *>(&HR)
            ->getOtherHalf()->getValueAnalysis();
        return &*VA->getMap()->find(VA->getValue());
      }
    };

  public:
    typedef forward_iterator_adapter<HalfRelationBaseList::iterator,
        ValueToAnalysisMap::value_type *, AdapterFunctor> iterator;
  };
}

// Graph traits specializations used exclusively for WriteGraph().
namespace llvm {
  template<>
  struct GraphTraits<ValueToAnalysisMap> {
    typedef const ValueToAnalysisMap::value_type NodeType;
    typedef ValueToAnalysisMapIteratorAdapter::iterator nodes_iterator;
    typedef OutgoingHalfRelationIteratorAdapter::iterator ChildIteratorType;

    static ChildIteratorType child_begin(NodeType *Node) {
      ValueAnalysis *VA = Node->second.getPtr();
      if (VA) {
        return ChildIteratorType(static_cast<HalfRelationList<OUTGOING> *>(VA)
            ->begin());
      } else {
        return ChildIteratorType();
      }
    }

    static ChildIteratorType child_end(NodeType *Node) {
      ValueAnalysis *VA = Node->second.getPtr();
      if (VA) {
        return ChildIteratorType(static_cast<HalfRelationList<OUTGOING> *>(VA)
            ->end());
      } else {
        return ChildIteratorType();
      }
    }

    static nodes_iterator nodes_begin(const ValueToAnalysisMap &Map) {
      return nodes_iterator(Map.begin());
    }

    static nodes_iterator nodes_end(const ValueToAnalysisMap &Map) {
      return nodes_iterator(Map.end());
    }
  };

  template<>
  struct DOTGraphTraits<ValueToAnalysisMap> : DefaultDOTGraphTraits {
    DOTGraphTraits(bool simple = false) : DefaultDOTGraphTraits(simple) {}

    std::string getNodeLabel(const ValueToAnalysisMap::value_type *Node,
        const ValueToAnalysisMap &Map) {
      const Value *V = Node->first;
      if (V->hasName()) {
        return V->getName();
      } else {
        std::ostringstream OSS;
        raw_os_ostream OS(OSS);
        V->print(OS);
        return OSS.str();
      }
    }
  };
}

void LazyAndersen::print(raw_ostream &OS, const Module *M) const {
  assert(Data);
  WriteGraph(OS, Data->ValueAnalyses, false,
      Twine("LazyAndersen analysis results for module ")
          + M->getModuleIdentifier());
}
