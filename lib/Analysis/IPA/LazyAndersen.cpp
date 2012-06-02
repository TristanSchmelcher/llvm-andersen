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
#include "llvm/Support/Debug.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/InstVisitor.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/User.h"
#include "llvm/Value.h"

#include <sstream>

using namespace llvm;

char LazyAndersen::ID = 0;
// TODO: What do these two bools mean?
INITIALIZE_PASS(LazyAndersen, "lazy-andersen",
                "Lazy Andersen's Algorithm for Points-To Analysis", false, true)

namespace {
  class HalfRelationBase;
  class LazyAnalysisStep;
  template<typename NodeTy> struct ilist_node_with_list_ptr_traits;

  template<typename NodeTy>
  class ilist_node_with_list_ptr : public ilist_node<NodeTy> {
    friend struct ilist_node_with_list_ptr_traits<NodeTy>;

    ilist<NodeTy> *List;

  protected:
    ilist_node_with_list_ptr() : List(0) {}
    ~ilist_node_with_list_ptr() {}

    ilist<NodeTy> *getList() const { return List; }

  private:
    void added(ilist<NodeTy> *ToList) {
      assert(!List);
      List = ToList;
    }

    void removed(ilist<NodeTy> *FromList) {
      assert(List == FromList);
      List = 0;
    }

    void transferred(ilist<NodeTy> *ToList, ilist<NodeTy> *FromList) {
      assert(List == FromList);
      List = ToList;
    }
  };

  template<typename NodeTy>
  struct ilist_node_with_list_ptr_traits : public ilist_default_traits<NodeTy> {
    void destroySentinel(NodeTy *) const {}

    NodeTy *provideInitialHead() const {
      return static_cast<const ilist_traits<NodeTy> *>(this)->createSentinel();
    }

    NodeTy *ensureHead(NodeTy *) const {
      return static_cast<const ilist_traits<NodeTy> *>(this)->createSentinel();
    }

    static void noteHead(NodeTy *, NodeTy *) {}

    void addNodeToList(NodeTy *Node) {
      Node->added(static_cast<ilist<NodeTy> *>(this));
    }

    void removeNodeFromList(NodeTy *Node) {
      Node->removed(static_cast<ilist<NodeTy> *>(this));
    }

    void transferNodesFromList(
        ilist_node_traits<NodeTy> &that,
        ilist_iterator<NodeTy> first,
        ilist_iterator<NodeTy> last) {
      for (ilist_iterator<NodeTy> i = first; i != last; ++i) {
        i->transferred(static_cast<ilist<NodeTy> *>(this),
            static_cast<ilist<NodeTy> *>(&that));
      }
    }
  };
}

namespace llvm {
  template<>
  struct ilist_traits<HalfRelationBase>
      : public ilist_node_with_list_ptr_traits<HalfRelationBase> {
  private:
    mutable ilist_half_node<HalfRelationBase> Sentinel;

  public:
    HalfRelationBase *createSentinel() const;
  };

  template<>
  struct ilist_traits<LazyAnalysisStep>
      : public ilist_node_with_list_ptr_traits<LazyAnalysisStep> {
  private:
    mutable ilist_half_node<LazyAnalysisStep> Sentinel;

  public:
    LazyAnalysisStep *createSentinel() const;
  };
}

namespace {
  typedef ilist<HalfRelationBase> HalfRelationBaseList;

  class HalfRelationBase :
      protected ilist_node_with_list_ptr<HalfRelationBase> {
    friend struct ilist_nextprev_traits<HalfRelationBase>;
    friend struct ilist_node_traits<HalfRelationBase>;
    friend struct ilist_node_with_list_ptr_traits<HalfRelationBase>;
    friend struct ilist_traits<HalfRelationBase>;

  protected:
    HalfRelationBase(HalfRelationBaseList *InitialList) {
      assert(InitialList);
      InitialList->push_back(this);
      assert(getList() == InitialList);
    }

    // virtual so that deleteNode in ilist_node_traits will delete the whole
    // Relation object properly.
    virtual ~HalfRelationBase() {
      if (getList()) {
        getList()->remove(HalfRelationBaseList::iterator(this));
        assert(!getList());
      }
    }
  };

  typedef ilist<LazyAnalysisStep> LazyAnalysisStepList;

  class LazyAnalysisStep : private ilist_node_with_list_ptr<LazyAnalysisStep> {
    friend struct ilist_nextprev_traits<LazyAnalysisStep>;
    friend struct ilist_node_traits<LazyAnalysisStep>;
    friend struct ilist_node_with_list_ptr_traits<LazyAnalysisStep>;
    friend struct ilist_traits<LazyAnalysisStep>;

  public:
    virtual ~LazyAnalysisStep() {}
  };
}

inline HalfRelationBase *ilist_traits<HalfRelationBase>::createSentinel()
    const {
  return static_cast<HalfRelationBase *>(&Sentinel);
}

inline LazyAnalysisStep *ilist_traits<LazyAnalysisStep>::createSentinel()
    const {
  return static_cast<LazyAnalysisStep *>(&Sentinel);
}

namespace {
  class LazyAnalysisResult : private RefCountedBase<LazyAnalysisResult>,
      public LazyAnalysisStepList {
    friend struct IntrusiveRefCntPtrInfo<LazyAnalysisResult>;
    friend class RefCountedBase<LazyAnalysisResult>;

  private:
    ~LazyAnalysisResult() {}
  };

  typedef IntrusiveRefCntPtr<LazyAnalysisResult> LazyAnalysisResultRef;

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
    LazyAnalysisResultRef PointsToSet;

  public:
    ValueAnalysis(const Value *V, ValueToAnalysisMap *Map) : V(V), Map(Map) {}

    const Value *getValue() const { return V; }
    ValueToAnalysisMap *getMap() const { return Map; }

  private:
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
      assert(getList());
      return static_cast<ValueAnalysis *>(
          static_cast<HalfRelationList<direction> *>(getList()));
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
   virtual const char *getRelationName() const = 0;
  };

  class DependsOnRelation : public Relation {
  public:
    DependsOnRelation(ValueAnalysis *DependentValueAnalyis,
        ValueAnalysis *IndependentValueAnalysis)
      : Relation(DependentValueAnalyis, IndependentValueAnalysis) {}

    virtual const char *getRelationName() const {
      return "depends on";
    }
  };

  class LoadedFromRelation : public Relation {
  public:
    LoadedFromRelation(ValueAnalysis *LoadedValueAnalyis,
        ValueAnalysis *AddressValueAnalysis)
      : Relation(LoadedValueAnalyis, AddressValueAnalysis) {}

    virtual const char *getRelationName() const {
      return "loaded from";
    }
  };

  class StoredToRelation : public Relation {
  public:
    StoredToRelation(ValueAnalysis *StoredValueAnalyis,
        ValueAnalysis *AddressValueAnalysis)
      : Relation(StoredValueAnalyis, AddressValueAnalysis) {}

    virtual const char *getRelationName() const {
      return "stored to";
    }
  };

  class ReturnedFromCalleeRelation : public Relation {
  public:
    ReturnedFromCalleeRelation(ValueAnalysis *ReturnedValueAnalyis,
        ValueAnalysis *CalledValueAnalysis)
      : Relation(ReturnedValueAnalyis, CalledValueAnalysis) {}

    virtual const char *getRelationName() const {
      return "returned from callee";
    }
  };

  class ArgumentToCalleeRelation : public Relation {
  public:
    ArgumentToCalleeRelation(ValueAnalysis *ArgumentValueAnalyis,
        ValueAnalysis *CalledValueAnalysis)
      : Relation(ArgumentValueAnalyis, CalledValueAnalysis) {}

    virtual const char *getRelationName() const {
      return "argument to callee";
    }
  };

  class ArgumentFromCallerRelation : public Relation {
  public:
    ArgumentFromCallerRelation(ValueAnalysis *ArgumentValueAnalyis,
        ValueAnalysis *FunctionValueAnalysis)
      : Relation(ArgumentValueAnalyis, FunctionValueAnalysis) {}

    virtual const char *getRelationName() const {
      return "argument from caller";
    }
  };

  class ReturnedToCallerRelation : public Relation {
  public:
    ReturnedToCallerRelation(ValueAnalysis *ReturnedValueAnalyis,
        ValueAnalysis *FunctionValueAnalysis)
      : Relation(ReturnedValueAnalyis, FunctionValueAnalysis) {}

    virtual const char *getRelationName() const {
      return "returned to caller";
    }
  };
}

namespace llvm {
  class LazyAndersenData {
  public:
    ValueToAnalysisMap ValueAnalyses;
  };
}

namespace {
  // Wrapper for the methods of CallInst and InvokeInst that ought to be
  // inherited from a common interface but aren't. :/
  class CallOrInvokeInstWrapperInterface {
  public:
    virtual const Value *getCalledValue() const = 0;
    virtual unsigned getNumArgOperands() const = 0;
    virtual const Value *getArgOperand(unsigned i) const = 0;
  };

  template<typename T>
  class CallOrInvokeInstWrapper : public CallOrInvokeInstWrapperInterface {
    const T &I;

  public:
    CallOrInvokeInstWrapper(const T &I) : I(I) {}
    virtual const Value *getCalledValue() const { return I.getCalledValue(); }
    virtual unsigned getNumArgOperands() const { return I.getNumArgOperands(); }
    virtual const Value *getArgOperand(unsigned i) const
        { return I.getArgOperand(i); }
  };

  class Analyze : private InstVisitor<Analyze> {
    typedef SmallVector<std::pair<const PHINode *, ValueAnalysis *>, 3>
        PHINodeWorkVector;
    friend class InstVisitor<Analyze>;
    PHINodeWorkVector PHINodeWork;
    LazyAndersenData *Data;
    Function *CurrentFunction;

  public:
    Analyze(Module &M) : Data(new LazyAndersenData()) {
      for (Module::iterator i = M.begin(); i != M.end(); ++i) {
        Function &F(*i);
        CurrentFunction = &F;
        visit(F);
        processPHINodes();
      }
    }

    LazyAndersenData *getResults() const { return Data; }

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
      visitCallOrInvokeInst(I, CallOrInvokeInstWrapper<InvokeInst>(I));
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
      ValueAnalysis *PHIAnalysis = cache(&I, createValueAnalysis(&I));
      PHINodeWork.push_back(PHINodeWorkVector::value_type(&I, PHIAnalysis));
    }

    void processPHINodes() {
      for (PHINodeWorkVector::const_iterator i = PHINodeWork.begin();
           i != PHINodeWork.end(); ++i) {
        const PHINode *PHI = i->first;
        ValueAnalysis *PHIAnalysis = i->second;
        for (PHINode::const_op_iterator i = PHI->op_begin(); i != PHI->op_end();
            ++i) {
          ValueAnalysis *OperandAnalysis = analyzeValue(*i);
          if (OperandAnalysis) {
            new DependsOnRelation(PHIAnalysis, OperandAnalysis);
          }
        }
      }
      PHINodeWork.clear();
    }

    void visitCallInst(CallInst &I) {
      visitCallOrInvokeInst(I, CallOrInvokeInstWrapper<CallInst>(I));
    }

    void visitVAArgInst(VAArgInst &I) {
      // TODO
    }

    void visitCallOrInvokeInst(Instruction &I,
        const CallOrInvokeInstWrapperInterface &W) {
      const Value *CalledValue = W.getCalledValue();
      ValueAnalysis *CalledValueAnalysis = analyzeValue(CalledValue);
      if (CalledValueAnalysis) {
        for (unsigned i = 0; i < W.getNumArgOperands(); i++) {
          const Value *ArgumentValue = W.getArgOperand(i);
          ValueAnalysis *ArgumentValueAnalysis = analyzeValue(ArgumentValue);
          if (ArgumentValueAnalysis) {
            new ArgumentToCalleeRelation(ArgumentValueAnalysis,
                CalledValueAnalysis);
          }
        }
      }
      if (!I.getType()->isVoidTy()) {
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
  Data = Analyze(M).getResults();
  print(dbgs(), &M);
  return false;
}

void LazyAndersen::releaseMemory() {
  delete Data;
  Data = 0;
}

void LazyAndersen::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
}

// Helpers used exclusively for the graph traits classes below.
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

    OriginalIteratorTy &wrappedIterator() { return i; }
    const OriginalIteratorTy &wrappedIterator() const { return i; }

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

  class IncomingHalfRelationIteratorAdapter {
    struct AdapterFunctor {
      ValueToAnalysisMap::value_type *operator()(const HalfRelationBase &HR) const {
        ValueAnalysis *VA = static_cast<const HalfRelation<INCOMING> *>(&HR)
            ->getOtherHalf()->getValueAnalysis();
        return &*VA->getMap()->find(VA->getValue());
      }
    };

  public:
    typedef forward_iterator_adapter<HalfRelationBaseList::iterator,
        ValueToAnalysisMap::value_type *, AdapterFunctor> iterator;
  };

  // Trim functions taken from
  // http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
  inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(
        std::ptr_fun<int, int>(std::isspace))));
    return s;
  }

  inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(
        std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }

  inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
  }
}

// Graph traits specializations used exclusively for WriteGraph().
namespace llvm {
  template<>
  struct GraphTraits<ValueToAnalysisMap> {
    typedef const ValueToAnalysisMap::value_type NodeType;
    typedef ValueToAnalysisMapIteratorAdapter::iterator nodes_iterator;
    typedef IncomingHalfRelationIteratorAdapter::iterator ChildIteratorType;

    static ChildIteratorType child_begin(NodeType *Node) {
      ValueAnalysis *VA = Node->second.getPtr();
      return ChildIteratorType(static_cast<HalfRelationList<INCOMING> *>(VA)
          ->begin());
    }

    static ChildIteratorType child_end(NodeType *Node) {
      ValueAnalysis *VA = Node->second.getPtr();
      return ChildIteratorType(static_cast<HalfRelationList<INCOMING> *>(VA)
          ->end());
    }

    static nodes_iterator nodes_begin(const ValueToAnalysisMap &Map) {
      return nodes_iterator(Map.begin());
    }

    static nodes_iterator nodes_end(const ValueToAnalysisMap &Map) {
      return nodes_iterator(Map.end());
    }
  };

  template<>
  class DOTGraphTraits<ValueToAnalysisMap> : public DefaultDOTGraphTraits {
    static std::string getPrintedValue(const Value *V) {
      std::ostringstream OSS;
      {
        raw_os_ostream OS(OSS);
        V->print(OS);
      }
      return OSS.str();
    }

    static const size_t MaxPrintedSize = 16;

  public:
    DOTGraphTraits(bool simple = false) : DefaultDOTGraphTraits(simple) {}

    std::string getNodeLabel(GraphTraits<ValueToAnalysisMap>::NodeType *Node,
        const ValueToAnalysisMap &Map) {
      const Value *V = Node->first;
      std::ostringstream OSS;
      {
        std::string PrintedValue(getPrintedValue(V));
        trim(PrintedValue);
        if (PrintedValue.size() > MaxPrintedSize) {
          PrintedValue.erase(MaxPrintedSize);
          rtrim(PrintedValue);
          OSS << PrintedValue << " ...";
        } else {
          OSS << PrintedValue;
        }
      }
      if (V->hasName()) {
        OSS << " (" << V->getName().str() << ')';
      }
      return OSS.str();
    }

    static std::string getEdgeSourceLabel(
        GraphTraits<ValueToAnalysisMap>::NodeType *Node,
        const GraphTraits<ValueToAnalysisMap>::ChildIteratorType &i) {
      return static_cast<Relation *>(static_cast<HalfRelation<INCOMING> *>(
          &*i.wrappedIterator()))->getRelationName();
    }

    static bool isNodeHidden(GraphTraits<ValueToAnalysisMap>::NodeType *Node) {
      return !Node->second.getPtr() || Node->first != Node->second->getValue();
    }
  };

#ifndef NDEBUG
  void viewLazyAndersenGraph(LazyAndersenData *Data) {
    ViewGraph(Data->ValueAnalyses, "LazyAndersen", false,
        "LazyAndersen analysis results");
  }
#endif
}

void LazyAndersen::print(raw_ostream &OS, const Module *M) const {
  assert(Data);
  WriteGraph(OS, Data->ValueAnalyses, false,
      Twine("LazyAndersen analysis results for module ")
          + M->getModuleIdentifier());
#ifndef NDEBUG
  // In debug mode, also display the graph.
  viewLazyAndersenGraph(Data);
#endif
}
