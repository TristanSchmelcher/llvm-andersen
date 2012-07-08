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

namespace llvm {
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

    template<typename NodeTy> struct ilist_node_with_list_ptr_traits;

    template<typename NodeTy>
    class ilist_node_with_list_ptr : public ilist_node<NodeTy> {
      friend struct ilist_node_with_list_ptr_traits<NodeTy>;

      ilist<NodeTy> *List;

    protected:
      ilist_node_with_list_ptr();
      ~ilist_node_with_list_ptr();
      ilist<NodeTy> *getList() const;

    private:
      void added(ilist<NodeTy> *ToList);
      void removed(ilist<NodeTy> *FromList);
      void transferred(ilist<NodeTy> *ToList, ilist<NodeTy> *FromList);
    };

    // class ilist_node_with_list_ptr
    template<typename NodeTy>
    ilist_node_with_list_ptr<NodeTy>::ilist_node_with_list_ptr() : List(0) {}

    template<typename NodeTy>
    ilist_node_with_list_ptr<NodeTy>::~ilist_node_with_list_ptr() {}

    template<typename NodeTy>
    inline ilist<NodeTy> *ilist_node_with_list_ptr<NodeTy>::getList() const {
      return List;
    }

    template<typename NodeTy>
    inline void ilist_node_with_list_ptr<NodeTy>::added(ilist<NodeTy> *ToList) {
      assert(!List);
      List = ToList;
    }

    template<typename NodeTy>
    inline void ilist_node_with_list_ptr<NodeTy>::removed(
        ilist<NodeTy> *FromList) {
      assert(List == FromList);
      List = 0;
    }

    template<typename NodeTy>
    inline void ilist_node_with_list_ptr<NodeTy>::transferred(
        ilist<NodeTy> *ToList, ilist<NodeTy> *FromList) {
      assert(List == FromList);
      List = ToList;
    }

    template<typename NodeTy>
    struct ilist_node_with_list_ptr_traits :
        public ilist_default_traits<NodeTy> {
      void destroySentinel(NodeTy *) const;
      NodeTy *provideInitialHead() const;
      NodeTy *ensureHead(NodeTy *) const;
      static void noteHead(NodeTy *, NodeTy *);
      void addNodeToList(NodeTy *Node);
      void removeNodeFromList(NodeTy *Node);
      void transferNodesFromList(
          ilist_node_traits<NodeTy> &that,
          ilist_iterator<NodeTy> first,
          ilist_iterator<NodeTy> last);
    };

    // struct ilist_node_with_list_ptr_traits
    template<typename NodeTy>
    inline void ilist_node_with_list_ptr_traits<NodeTy>::destroySentinel(
        NodeTy *) const {}

    template<typename NodeTy>
    inline NodeTy *ilist_node_with_list_ptr_traits<NodeTy>::provideInitialHead()
        const {
      return static_cast<const ilist_traits<NodeTy> *>(this)->createSentinel();
    }

    template<typename NodeTy>
    inline NodeTy *ilist_node_with_list_ptr_traits<NodeTy>::ensureHead(NodeTy *)
        const {
      return static_cast<const ilist_traits<NodeTy> *>(this)->createSentinel();
    }

    template<typename NodeTy>
    inline void ilist_node_with_list_ptr_traits<NodeTy>::noteHead(NodeTy *,
        NodeTy *) {}

    template<typename NodeTy>
    inline void ilist_node_with_list_ptr_traits<NodeTy>::addNodeToList(
        NodeTy *Node) {
      Node->added(static_cast<ilist<NodeTy> *>(this));
    }

    template<typename NodeTy>
    inline void ilist_node_with_list_ptr_traits<NodeTy>::removeNodeFromList(
        NodeTy *Node) {
      Node->removed(static_cast<ilist<NodeTy> *>(this));
    }

    template<typename NodeTy>
    void ilist_node_with_list_ptr_traits<NodeTy>::transferNodesFromList(
        ilist_node_traits<NodeTy> &that,
        ilist_iterator<NodeTy> first,
        ilist_iterator<NodeTy> last) {
      for (ilist_iterator<NodeTy> i = first; i != last; ++i) {
        i->transferred(static_cast<ilist<NodeTy> *>(this),
            static_cast<ilist<NodeTy> *>(&that));
      }
    }

    template<RelationDirection direction> class HalfRelation;
    class HalfRelationBaseList;

    class HalfRelationBase :
        private ilist_node_with_list_ptr<HalfRelationBase> {
      friend struct ilist_nextprev_traits<HalfRelationBase>;
      friend struct ilist_node_traits<HalfRelationBase>;
      friend struct ilist_node_with_list_ptr_traits<HalfRelationBase>;
      friend struct ilist_traits<HalfRelationBase>;
      template<RelationDirection> friend class HalfRelation;

    public:
      template<RelationDirection direction>
      const HalfRelation<direction> *as() const;

      template<RelationDirection direction>
      HalfRelation<direction> *as();

    protected:
      HalfRelationBaseList *getList() const;

    private:
      explicit HalfRelationBase(HalfRelationBaseList *InitialList);
      // virtual so that deleteNode in ilist_node_traits will delete the whole
      // Relation object properly.
      virtual ~HalfRelationBase();
    };
  }

  template<>
  struct ilist_traits<HalfRelationBase>
      : public ilist_node_with_list_ptr_traits<HalfRelationBase> {
  private:
    // Have to duplicate the Sentinel logic for each specialization because the
    // ilist_half_node constructor is protected and only friends with
    // ilist_traits.
    mutable ilist_half_node<HalfRelationBase> Sentinel;

  public:
    HalfRelationBase *createSentinel() const;
  };

  inline HalfRelationBase *ilist_traits<HalfRelationBase>::createSentinel()
      const {
    return static_cast<HalfRelationBase *>(&Sentinel);
  }

  namespace {
    class Relation;
    class ValueAnalysis;

    template<RelationDirection direction>
    class HalfRelation : public HalfRelationBase,
        private direction_traits<direction> {
      friend class Relation;

    public:
      using direction_traits<direction>::OppositeDirection;
      typedef HalfRelation<OppositeDirection> OppositeHalfRelationTy;

      const Relation *getRelation() const;
      Relation *getRelation();
      ValueAnalysis *getValueAnalysis() const;
      const OppositeHalfRelationTy *getOppositeDirection() const;
      OppositeHalfRelationTy *getOppositeDirection();

    private:
      explicit HalfRelation(ValueAnalysis *VA);
      ~HalfRelation();
    };

    template<RelationDirection direction> class HalfRelationList;

    class HalfRelationBaseList : public ilist<HalfRelationBase> {
      template<RelationDirection direction> friend class HalfRelationList;

    public:
      template<RelationDirection direction>
      const HalfRelationList<direction> *as() const;

      template<RelationDirection direction>
      HalfRelationList<direction> *as();

    private:
      HalfRelationBaseList();
    };

    // class HalfRelationBase
    template<RelationDirection direction>
    inline const HalfRelation<direction> *HalfRelationBase::as() const {
      return static_cast<const HalfRelation<direction> *>(this);
    }

    template<RelationDirection direction>
    inline HalfRelation<direction> *HalfRelationBase::as() {
      return static_cast<HalfRelation<direction> *>(this);
    }

    inline HalfRelationBaseList *HalfRelationBase::getList() const {
      return static_cast<HalfRelationBaseList *>(
          ilist_node_with_list_ptr::getList());
    }

    HalfRelationBase::HalfRelationBase(
        HalfRelationBaseList *InitialList) {
      assert(InitialList);
      InitialList->push_back(this);
      assert(getList() == InitialList);
    }

    HalfRelationBase::~HalfRelationBase() {
      if (getList()) {
        getList()->remove(HalfRelationBaseList::iterator(this));
        assert(!getList());
      }
    }

    template<RelationDirection direction>
    class HalfRelationList : public HalfRelationBaseList {
      friend class ValueAnalysis;

    public:
      const ValueAnalysis *getValueAnalysis() const;
      ValueAnalysis *getValueAnalysis();

    private:
      HalfRelationList();
    };

    // class HalfRelationBaseList
    template<RelationDirection direction>
    inline const HalfRelationList<direction> *HalfRelationBaseList::as() const {
      return static_cast<const HalfRelationList<direction> *>(this);
    }

    template<RelationDirection direction>
    inline HalfRelationList<direction> *HalfRelationBaseList::as() {
      return static_cast<HalfRelationList<direction> *>(this);
    }

    HalfRelationBaseList::HalfRelationBaseList() {}

    // A Relation is a directed edge in the analysis graph.
    class Relation :
        private HalfRelation<INCOMING>,
        private HalfRelation<OUTGOING> {
      template<RelationDirection direction> friend class HalfRelation;

    public:
      Relation(ValueAnalysis *Src, ValueAnalysis *Dst);
      virtual ~Relation();
      virtual const char *getRelationName() const = 0;

      template<RelationDirection direction>
      const HalfRelation<direction> *getDirection() const;

      template<RelationDirection direction>
      HalfRelation<direction> *getDirection();
    };

    // class Relation
    Relation::Relation(ValueAnalysis *Src, ValueAnalysis *Dst)
      : HalfRelation<INCOMING>(Src), HalfRelation<OUTGOING>(Dst) {}

    Relation::~Relation() {}

    template<RelationDirection direction>
    inline const HalfRelation<direction> *Relation::getDirection() const {
      return static_cast<const HalfRelation<direction> *>(this);
    }

    template<RelationDirection direction>
    inline HalfRelation<direction> *Relation::getDirection() {
      return static_cast<HalfRelation<direction> *>(this);
    }

    class LazyAnalysisStep :
        protected ilist_node_with_list_ptr<LazyAnalysisStep> {
      friend struct ilist_nextprev_traits<LazyAnalysisStep>;
      friend struct ilist_node_traits<LazyAnalysisStep>;
      friend struct ilist_node_with_list_ptr_traits<LazyAnalysisStep>;
      friend struct ilist_traits<LazyAnalysisStep>;

    public:
      // 2-tuple of next ValueAnalysis result and next LazyAnalysisStep to run
      // to produce the following results. One or both may be 0.
      typedef std::pair<ValueAnalysis *, LazyAnalysisStep *> Result;

      virtual ~LazyAnalysisStep();
      virtual Result run() = 0;

    protected:
      // Get the step after this in the list, or 0 if none.
      LazyAnalysisStep *getNextStep();
    };
  }

  template<>
  struct ilist_traits<LazyAnalysisStep>
      : public ilist_node_with_list_ptr_traits<LazyAnalysisStep> {
  private:
    // Have to duplicate the Sentinel logic for each specialization because the
    // ilist_half_node constructor is protected and only friends with
    // ilist_traits.
    mutable ilist_half_node<LazyAnalysisStep> Sentinel;

  public:
    LazyAnalysisStep *createSentinel() const;
  };

  inline LazyAnalysisStep *ilist_traits<LazyAnalysisStep>::createSentinel()
      const {
    return static_cast<LazyAnalysisStep *>(&Sentinel);
  }

  namespace {
    typedef ilist<LazyAnalysisStep> LazyAnalysisStepList;

    // class LazyAnalysisStep
    LazyAnalysisStep::~LazyAnalysisStep() {
      assert(!getList());
    }

    LazyAnalysisStep *LazyAnalysisStep::getNextStep() {
      assert(getList());
      LazyAnalysisStepList::iterator i(this);
      ++i;
      if (i == getList()->end()) {
        return 0;
      } else {
        return &*i;
      }
    }

    class LazyAnalysisResult : private RefCountedBase<LazyAnalysisResult>,
        public LazyAnalysisStepList {
      friend struct IntrusiveRefCntPtrInfo<LazyAnalysisResult>;
      friend class RefCountedBase<LazyAnalysisResult>;

    public:
      typedef IntrusiveRefCntPtr<LazyAnalysisResult> Ref;

    private:
      ~LazyAnalysisResult();
    };

    // class LazyAnalysisResult
    LazyAnalysisResult::~LazyAnalysisResult() {}

    class IterativeAnalysisStep : public LazyAnalysisStep {
    protected:
      // Push the next iteration onto the step list and return it as the result.
      Result nextIteration(LazyAnalysisStep *Step);
      // Pop this completed step off the stack (thus deleting it) and return the
      // next one.
      Result done();
    };

    // class IterativeAnalysisStep
    IterativeAnalysisStep::Result IterativeAnalysisStep::nextIteration(
        LazyAnalysisStep *Step) {
      assert(getList());
      getList()->insert(LazyAnalysisStepList::iterator(this), Step);
      return Result(0, Step);
    }

    IterativeAnalysisStep::Result IterativeAnalysisStep::done() {
      assert(getList());
      LazyAnalysisStep *Next = getNextStep();
      // erase deletes this object.
      getList()->erase(this);
      return Result(0, Next);
    }

    // Helper class for analyzing relations in a particular direction.
    class RelationsAnalysisStepBase : public IterativeAnalysisStep {
      HalfRelationBaseList *List;
      HalfRelationBaseList::iterator i;

    public:
      explicit RelationsAnalysisStepBase(HalfRelationBaseList *List);
      virtual Result run();

    private:
      virtual LazyAnalysisStep *analyzeHalfRelation(HalfRelationBase *HR) = 0;
    };

    // class RelationsAnalysisStepBase
    RelationsAnalysisStepBase::RelationsAnalysisStepBase(
        HalfRelationBaseList *List)
      : List(List), i(List->begin()) {}

    RelationsAnalysisStepBase::Result RelationsAnalysisStepBase::run() {
      while (i != List->end()) {
        HalfRelationBase *HR = &*i;
        ++i;
        LazyAnalysisStep *LA = analyzeHalfRelation(HR);
        if (LA) {
          return nextIteration(LA);
        }
        // Else nothing to do for this relation; keep going.
      }
      // No relations left to analyze.
      return done();
    }

    template<RelationDirection direction, typename StepTy>
    class RelationsAnalysisStep : public RelationsAnalysisStepBase {
    public:
      explicit RelationsAnalysisStep(ValueAnalysis *VA);

    private:
      virtual LazyAnalysisStep *analyzeHalfRelation(HalfRelationBase *HR);
    };

    class PointsToRelationsAnalysisStep : public RelationsAnalysisStep<OUTGOING,
        PointsToRelationsAnalysisStep> {
      friend class RelationsAnalysisStep<OUTGOING,
          PointsToRelationsAnalysisStep>;

    private:
      LazyAnalysisStep *analyzeRelation(Relation *R);
    };

    // class PointsToRelationsAnalysisStep
    LazyAnalysisStep *PointsToRelationsAnalysisStep::analyzeRelation(
        Relation *R) {
      // TODO
      return 0;
    }

    class ValueAnalysis : private RefCountedBase<ValueAnalysis>,
        private HalfRelationList<INCOMING>,
        private HalfRelationList<OUTGOING> {
      friend struct IntrusiveRefCntPtrInfo<ValueAnalysis>;
      friend class RefCountedBase<ValueAnalysis>;
      template<RelationDirection direction> friend class HalfRelationList;
      // The Value that maps to this object. (If this analysis applies to
      // multiple Values, this is the first one that was analyzed.)
      const Value *V;

    public:
      typedef IntrusiveRefCntPtr<ValueAnalysis> Ref;
      // TODO: Should this be a ValueMap?
      typedef DenseMap<const Value *, Ref> Map;

    private:
      // The map that this analysis is in.
      Map *ContainingMap;
      LazyAnalysisResult::Ref PointsToSet;

    public:
      static ValueAnalysis *const Nil;

      ValueAnalysis(const Value *V, Map *Map);
      const Value *getValue() const;
      Map *getMap() const;

      template<RelationDirection direction>
      const HalfRelationList<direction> *getRelations() const;

      template<RelationDirection direction>
      HalfRelationList<direction> *getRelations();

    private:
      ~ValueAnalysis();
    };

    // class ValueAnalysis
    ValueAnalysis *const ValueAnalysis::Nil = 0;

    ValueAnalysis::ValueAnalysis(const Value *V, Map *ContainingMap)
      : V(V), ContainingMap(ContainingMap) {}

    inline const Value *ValueAnalysis::getValue() const {
      return V;
    }

    inline ValueAnalysis::Map *ValueAnalysis::getMap() const {
      return ContainingMap;
    }

    template<RelationDirection direction>
    inline const HalfRelationList<direction> *ValueAnalysis::getRelations()
        const {
      return static_cast<const HalfRelationList<direction> *>(this);
    }

    template<RelationDirection direction>
    inline HalfRelationList<direction> *ValueAnalysis::getRelations() {
      return static_cast<HalfRelationList<direction> *>(this);
    }

    ValueAnalysis::~ValueAnalysis() {}

    // class HalfRelation
    template<RelationDirection direction>
    inline const Relation *HalfRelation<direction>::getRelation() const {
      return static_cast<const Relation *>(this);
    }

    template<RelationDirection direction>
    inline Relation *HalfRelation<direction>::getRelation() {
      return static_cast<Relation *>(this);
    }

    template<RelationDirection direction>
    ValueAnalysis *HalfRelation<direction>::getValueAnalysis() const {
      assert(getList());
      return getList()->as<direction>()->getValueAnalysis();
    }

    template<RelationDirection direction>
    const typename HalfRelation<direction>::OppositeHalfRelationTy *
    HalfRelation<direction>::getOppositeDirection() const {
      return getRelation()->template getDirection<OppositeDirection>();
    }

    template<RelationDirection direction>
    typename HalfRelation<direction>::OppositeHalfRelationTy *
    HalfRelation<direction>::getOppositeDirection() {
      return getRelation()->template getDirection<OppositeDirection>();
    }

    template<RelationDirection direction>
    inline HalfRelation<direction>::HalfRelation(ValueAnalysis *VA)
      : HalfRelationBase(VA->getRelations<direction>()) {}

    template<RelationDirection direction>
    HalfRelation<direction>::~HalfRelation() {}

    // class HalfRelationList
    template<RelationDirection direction>
    inline const ValueAnalysis *HalfRelationList<direction>::getValueAnalysis()
        const {
      return static_cast<const ValueAnalysis *>(this);
    }

    template<RelationDirection direction>
    inline ValueAnalysis *HalfRelationList<direction>::getValueAnalysis() {
      return static_cast<ValueAnalysis *>(this);
    }

    template<RelationDirection direction>
    HalfRelationList<direction>::HalfRelationList() {}

    // class RelationsAnalysisStep
    template<RelationDirection direction, typename StepTy>
    RelationsAnalysisStep<direction, StepTy>::RelationsAnalysisStep(
        ValueAnalysis *VA)
      : RelationsAnalysisStepBase(VA->getRelations<direction>()) {}

    template<RelationDirection direction, typename StepTy>
    LazyAnalysisStep *RelationsAnalysisStep<direction, StepTy>::
        analyzeHalfRelation(HalfRelationBase *HR) {
      return static_cast<StepTy *>(this)->analyzeRelation(
          HR->as<direction>()->getRelation());
    }
  }

  class LazyAndersenData {
  public:
    ValueAnalysis::Map ValueAnalyses;
  };

  namespace {
    class DependsOnRelation : public Relation {
    public:
      DependsOnRelation(ValueAnalysis *DependentValueAnalyis,
          ValueAnalysis *IndependentValueAnalysis);
      virtual const char *getRelationName() const;
    };

    // class DependsOnRelation
    inline DependsOnRelation::DependsOnRelation(
        ValueAnalysis *DependentValueAnalyis,
        ValueAnalysis *IndependentValueAnalysis)
      : Relation(DependentValueAnalyis, IndependentValueAnalysis) {}

    const char *DependsOnRelation::getRelationName() const {
      return "depends on";
    }

    class LoadedFromRelation : public Relation {
    public:
      LoadedFromRelation(ValueAnalysis *LoadedValueAnalyis,
          ValueAnalysis *AddressValueAnalysis);
      virtual const char *getRelationName() const;
    };

    // class LoadedFromRelation
    inline LoadedFromRelation::LoadedFromRelation(
        ValueAnalysis *LoadedValueAnalyis, ValueAnalysis *AddressValueAnalysis)
      : Relation(LoadedValueAnalyis, AddressValueAnalysis) {}

    const char *LoadedFromRelation::getRelationName() const {
      return "loaded from";
    }

    class StoredToRelation : public Relation {
    public:
      StoredToRelation(ValueAnalysis *StoredValueAnalyis,
          ValueAnalysis *AddressValueAnalysis);
      virtual const char *getRelationName() const;
    };

    // class StoredToRelation
    inline StoredToRelation::StoredToRelation(ValueAnalysis *StoredValueAnalyis,
        ValueAnalysis *AddressValueAnalysis)
      : Relation(StoredValueAnalyis, AddressValueAnalysis) {}

    const char *StoredToRelation::getRelationName() const {
      return "stored to";
    }

    class ReturnedFromCalleeRelation : public Relation {
    public:
      ReturnedFromCalleeRelation(ValueAnalysis *ReturnedValueAnalyis,
          ValueAnalysis *CalledValueAnalysis);
      virtual const char *getRelationName() const;
    };

    // class ReturnedFromCalleeRelation
    inline ReturnedFromCalleeRelation::ReturnedFromCalleeRelation(
        ValueAnalysis *ReturnedValueAnalyis, ValueAnalysis *CalledValueAnalysis)
      : Relation(ReturnedValueAnalyis, CalledValueAnalysis) {}

    const char *ReturnedFromCalleeRelation::getRelationName() const {
      return "returned from callee";
    }

    class ArgumentToCalleeRelation : public Relation {
    public:
      ArgumentToCalleeRelation(ValueAnalysis *ArgumentValueAnalyis,
          ValueAnalysis *CalledValueAnalysis);
      virtual const char *getRelationName() const;
    };

    // class ArgumentToCalleeRelation
    inline ArgumentToCalleeRelation::ArgumentToCalleeRelation(
        ValueAnalysis *ArgumentValueAnalyis, ValueAnalysis *CalledValueAnalysis)
      : Relation(ArgumentValueAnalyis, CalledValueAnalysis) {}

    const char *ArgumentToCalleeRelation::getRelationName() const {
      return "argument to callee";
    }

    class ArgumentFromCallerRelation : public Relation {
    public:
      ArgumentFromCallerRelation(ValueAnalysis *ArgumentValueAnalyis,
          ValueAnalysis *FunctionValueAnalysis);
      virtual const char *getRelationName() const;
    };

    // class ArgumentFromCallerRelation
    inline ArgumentFromCallerRelation::ArgumentFromCallerRelation(
        ValueAnalysis *ArgumentValueAnalyis,
        ValueAnalysis *FunctionValueAnalysis)
      : Relation(ArgumentValueAnalyis, FunctionValueAnalysis) {}

    const char *ArgumentFromCallerRelation::getRelationName() const {
      return "argument from caller";
    }

    class ReturnedToCallerRelation : public Relation {
    public:
      ReturnedToCallerRelation(ValueAnalysis *ReturnedValueAnalyis,
          ValueAnalysis *FunctionValueAnalysis);
      virtual const char *getRelationName() const;
    };

    // class ReturnedToCallerRelation
    inline ReturnedToCallerRelation::ReturnedToCallerRelation(
        ValueAnalysis *ReturnedValueAnalyis,
        ValueAnalysis *FunctionValueAnalysis)
      : Relation(ReturnedValueAnalyis, FunctionValueAnalysis) {}

    const char *ReturnedToCallerRelation::getRelationName() const {
      return "returned to caller";
    }

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

    class Analyze : public InstVisitor<Analyze> {
      typedef SmallVector<std::pair<const PHINode *, ValueAnalysis *>, 3>
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
      explicit Analyze(Module &M);
      void visitCallOrInvokeInst(Instruction &I,
          const CallOrInvokeInstWrapperInterface &W);
      void processPHINodes();
      bool analyzed(const Value *V);
      ValueAnalysis *cache(const Value *V, ValueAnalysis *VA);
      ValueAnalysis *createValueAnalysis(const Value *V);
      ValueAnalysis *createFinalizedValueAnalysis(const Value *V);
      ValueAnalysis *analyzeValue(const Value *V);
      ValueAnalysis *analyzeGlobalValue(const GlobalValue *G);
      ValueAnalysis *analyzeArgument(const Argument *A);
      ValueAnalysis *analyzeUser(const User *U);
    };

    // class Analyze
    LazyAndersenData *Analyze::run(Module &M) {
      return Analyze(M).Data;
    }

    void Analyze::visitReturnInst(ReturnInst &I) {
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

    void Analyze::visitInvokeInst(InvokeInst &I) {
      visitCallOrInvokeInst(I, CallOrInvokeInstWrapper<InvokeInst>(I));
    }

    void Analyze::visitAllocaInst(AllocaInst &I) {
      cache(&I, createFinalizedValueAnalysis(&I));
    }

    void Analyze::visitLoadInst(LoadInst &I) {
      ValueAnalysis *AddressAnalysis = analyzeValue(I.getPointerOperand());
      if (AddressAnalysis) {
        ValueAnalysis *LoadedValueAnalysis = cache(&I, createValueAnalysis(&I));
        new LoadedFromRelation(LoadedValueAnalysis, AddressAnalysis);
      } else {
        cache(&I, ValueAnalysis::Nil);
      }
      // TODO: Record that the current function may load this address.
    }

    void Analyze::visitStoreInst(StoreInst &I) {
      ValueAnalysis *AddressAnalysis = analyzeValue(I.getPointerOperand());
      if (AddressAnalysis) {
        ValueAnalysis *StoredValueAnalysis = analyzeValue(I.getValueOperand());
        if (StoredValueAnalysis) {
          new StoredToRelation(StoredValueAnalysis, AddressAnalysis);
          // TODO: Record that the current function may store this address.
        }
      }
    }

    void Analyze::visitAtomicCmpXchgInst(AtomicCmpXchgInst &I) {
      // TODO
    }

    void Analyze::visitAtomicRMWInst(AtomicRMWInst &I) {
      // TODO
    }

    void Analyze::visitPHINode(PHINode &I) {
      ValueAnalysis *PHIAnalysis = cache(&I, createValueAnalysis(&I));
      PHINodeWork.push_back(PHINodeWorkVector::value_type(&I, PHIAnalysis));
    }

    void Analyze::visitCallInst(CallInst &I) {
      visitCallOrInvokeInst(I, CallOrInvokeInstWrapper<CallInst>(I));
    }

    void Analyze::visitVAArgInst(VAArgInst &I) {
      // TODO
    }

    void Analyze::visitInstruction(Instruction &I) {
      assert(!I.mayReadOrWriteMemory() && "Unhandled memory instruction");
      cache(&I, analyzeUser(&I));
    }


    Analyze::Analyze(Module &M) : Data(new LazyAndersenData()) {
      for (Module::iterator i = M.begin(); i != M.end(); ++i) {
        Function &F(*i);
        CurrentFunction = &F;
        visit(F);
        processPHINodes();
      }
    }

    void Analyze::visitCallOrInvokeInst(Instruction &I,
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
          ReturnedValueAnalysis = ValueAnalysis::Nil;
        }
        cache(&I, ReturnedValueAnalysis);
      }
      // TODO: Record that the current function may call this function.
    }

    void Analyze::processPHINodes() {
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

    bool Analyze::analyzed(const Value *V) {
      return Data->ValueAnalyses.count(V);
    }

    ValueAnalysis *Analyze::cache(const Value *V, ValueAnalysis *VA) {
      assert(!analyzed(V));
      Data->ValueAnalyses[V] = VA;
      return VA;
    }

    ValueAnalysis *Analyze::createValueAnalysis(const Value *V) {
      return new ValueAnalysis(V, &Data->ValueAnalyses);
    }

    ValueAnalysis *Analyze::createFinalizedValueAnalysis(const Value *V) {
      // TODO: Pass finalized points-to set as argument and pre-create the set.
      return createValueAnalysis(V);
    }

    ValueAnalysis *Analyze::analyzeValue(const Value *V) {
      ValueAnalysis::Map::const_iterator i = Data->ValueAnalyses.find(V);
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
        VA = ValueAnalysis::Nil;
      }
      return cache(V, VA);
    }

    ValueAnalysis *Analyze::analyzeGlobalValue(const GlobalValue *G) {
      // TODO: Need to be aware of linkage here. Also, GlobalAlias may be
      // special. Also, a global might be initialized with a value that points
      // to something else.
      return createFinalizedValueAnalysis(G);
    }

    ValueAnalysis *Analyze::analyzeArgument(const Argument *A) {
      ValueAnalysis *ArgumentValueAnalysis = createValueAnalysis(A);
      ValueAnalysis *FunctionValueAnalysis = analyzeValue(CurrentFunction);
      new ArgumentFromCallerRelation(ArgumentValueAnalysis,
          FunctionValueAnalysis);
      return ArgumentValueAnalysis;
    }

    ValueAnalysis *Analyze::analyzeUser(const User *U) {
      typedef SmallVector<ValueAnalysis *, 3> ValueAnalysisVector;
      ValueAnalysisVector Set;
      for (User::const_op_iterator i = U->op_begin(); i != U->op_end(); ++i) {
        ValueAnalysis *VA = analyzeValue(*i);
        if (VA != ValueAnalysis::Nil) {
          Set.push_back(VA);
        }
      }
      ValueAnalysis *Result;
      switch (Set.size()) {
      case 0:
        Result = ValueAnalysis::Nil;
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

    // Helpers for graph traits specialization.

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

      forward_iterator_adapter();
      explicit forward_iterator_adapter(const OriginalIteratorTy &i);
      OriginalIteratorTy &wrappedIterator();
      const OriginalIteratorTy &wrappedIterator() const;
      value_type operator*() const;
      bool operator==(const forward_iterator_adapter &that) const;
      bool operator!=(const forward_iterator_adapter &that) const;
      forward_iterator_adapter &operator++();
      forward_iterator_adapter operator++(int);
    };

    // class forward_iterator_adapter
    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    forward_iterator_adapter<OriginalIteratorTy, ValueTy, AdapterFunctorTy>::
        forward_iterator_adapter() {}

    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    forward_iterator_adapter<OriginalIteratorTy, ValueTy, AdapterFunctorTy>::
        forward_iterator_adapter(const OriginalIteratorTy &i) : i(i) {}

    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    inline OriginalIteratorTy &forward_iterator_adapter<OriginalIteratorTy,
        ValueTy, AdapterFunctorTy>::wrappedIterator() {
      return i;
    }

    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    inline const OriginalIteratorTy &forward_iterator_adapter<
        OriginalIteratorTy, ValueTy, AdapterFunctorTy>::
        wrappedIterator() const {
      return i;
    }

    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    typename forward_iterator_adapter<OriginalIteratorTy, ValueTy,
        AdapterFunctorTy>::value_type
    forward_iterator_adapter<OriginalIteratorTy, ValueTy, AdapterFunctorTy>::
        operator*() const {
      return AdapterFunctor(*i);
    }

    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    inline bool forward_iterator_adapter<OriginalIteratorTy, ValueTy,
        AdapterFunctorTy>::operator==(const forward_iterator_adapter &that)
        const {
      return i == that.i;
    }

    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    inline bool forward_iterator_adapter<OriginalIteratorTy, ValueTy,
        AdapterFunctorTy>::operator!=(const forward_iterator_adapter &that)
        const {
      return i != that.i;
    }

    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    forward_iterator_adapter<OriginalIteratorTy, ValueTy, AdapterFunctorTy> &
    forward_iterator_adapter<OriginalIteratorTy, ValueTy, AdapterFunctorTy>::
        operator++() {
      ++i;
      return *this;
    }

    template<typename OriginalIteratorTy, typename ValueTy,
        typename AdapterFunctorTy>
    forward_iterator_adapter<OriginalIteratorTy, ValueTy, AdapterFunctorTy>
    forward_iterator_adapter<OriginalIteratorTy, ValueTy, AdapterFunctorTy>::
        operator++(int) {
      forward_iterator_adapter tmp = *this; ++*this; return tmp;
    }

    class ValueAnalysisMapIteratorAdapter {
      struct AdapterFunctor {
        const ValueAnalysis::Map::value_type *operator()(
            const ValueAnalysis::Map::value_type &value) const;
      };

    public:
      typedef forward_iterator_adapter<ValueAnalysis::Map::const_iterator,
          const ValueAnalysis::Map::value_type *, AdapterFunctor> iterator;
    };

    // class ValueAnalysisMapIteratorAdapter::AdapterFunctor
    inline const ValueAnalysis::Map::value_type *
    ValueAnalysisMapIteratorAdapter::AdapterFunctor::operator()(
        const ValueAnalysis::Map::value_type &value) const {
      return &value;
    }

    class IncomingHalfRelationIteratorAdapter {
      struct AdapterFunctor {
        ValueAnalysis::Map::value_type *operator()(const HalfRelationBase &HR)
            const;
      };

    public:
      typedef forward_iterator_adapter<HalfRelationBaseList::iterator,
          ValueAnalysis::Map::value_type *, AdapterFunctor> iterator;
    };

    // class IncomingHalfRelationIteratorAdapter::AdapterFunctor
    ValueAnalysis::Map::value_type *IncomingHalfRelationIteratorAdapter::
        AdapterFunctor::operator()(const HalfRelationBase &HR) const {
      ValueAnalysis *VA = HR.as<INCOMING>()->getOppositeDirection()
          ->getValueAnalysis();
      return &*VA->getMap()->find(VA->getValue());
    }

    // Trim functions taken from
    // http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    std::string &ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(
          std::ptr_fun<int, int>(std::isspace))));
      return s;
    }

    std::string &rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(
          std::ptr_fun<int, int>(std::isspace))).base(), s.end());
      return s;
    }

    std::string &trim(std::string &s) {
      return ltrim(rtrim(s));
    }
  }

  // Graph traits specializations for WriteGraph().

  template<>
  struct GraphTraits<ValueAnalysis::Map> {
    typedef const ValueAnalysis::Map::value_type NodeType;
    typedef ValueAnalysisMapIteratorAdapter::iterator nodes_iterator;
    typedef IncomingHalfRelationIteratorAdapter::iterator ChildIteratorType;

    static ChildIteratorType child_begin(NodeType *Node);
    static ChildIteratorType child_end(NodeType *Node);
    static nodes_iterator nodes_begin(const ValueAnalysis::Map &Map);
    static nodes_iterator nodes_end(const ValueAnalysis::Map &Map);
  };

  // struct GraphTraits
  GraphTraits<ValueAnalysis::Map>::ChildIteratorType
  GraphTraits<ValueAnalysis::Map>::child_begin(
      NodeType *Node) {
    ValueAnalysis *VA = Node->second.getPtr();
    return ChildIteratorType(VA->getRelations<INCOMING>()->begin());
  }

  GraphTraits<ValueAnalysis::Map>::ChildIteratorType
  GraphTraits<ValueAnalysis::Map>::child_end(NodeType *Node) {
    ValueAnalysis *VA = Node->second.getPtr();
    return ChildIteratorType(VA->getRelations<INCOMING>()->end());
  }

  GraphTraits<ValueAnalysis::Map>::nodes_iterator
  GraphTraits<ValueAnalysis::Map>::nodes_begin(
      const ValueAnalysis::Map &Map) {
    return nodes_iterator(Map.begin());
  }

  GraphTraits<ValueAnalysis::Map>::nodes_iterator
  GraphTraits<ValueAnalysis::Map>::nodes_end(
      const ValueAnalysis::Map &Map) {
    return nodes_iterator(Map.end());
  }

  template<>
  class DOTGraphTraits<ValueAnalysis::Map> : public DefaultDOTGraphTraits {
  public:
    DOTGraphTraits(bool simple = false);
    std::string getNodeLabel(GraphTraits<ValueAnalysis::Map>::NodeType *Node,
        const ValueAnalysis::Map &Map);
    static std::string getEdgeSourceLabel(
        GraphTraits<ValueAnalysis::Map>::NodeType *Node,
        const GraphTraits<ValueAnalysis::Map>::ChildIteratorType &i);
    static bool isNodeHidden(GraphTraits<ValueAnalysis::Map>::NodeType *Node);
  };

  // class DOTGraphTraits
  DOTGraphTraits<ValueAnalysis::Map>::DOTGraphTraits(bool simple)
    : DefaultDOTGraphTraits(simple) {}

  namespace {
    inline std::string getPrintedValue(const Value *V) {
      std::ostringstream OSS;
      {
        raw_os_ostream OS(OSS);
        V->print(OS);
      }
      return OSS.str();
    }
  }

  std::string DOTGraphTraits<ValueAnalysis::Map>::getNodeLabel(
      GraphTraits<ValueAnalysis::Map>::NodeType *Node,
      const ValueAnalysis::Map &Map) {
    static const size_t MaxPrintedSize = 16;

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

  std::string DOTGraphTraits<ValueAnalysis::Map>::getEdgeSourceLabel(
      GraphTraits<ValueAnalysis::Map>::NodeType *Node,
      const GraphTraits<ValueAnalysis::Map>::ChildIteratorType &i) {
    return i.wrappedIterator()->as<INCOMING>()->getRelation()
        ->getRelationName();
  }

  bool DOTGraphTraits<ValueAnalysis::Map>::isNodeHidden(
      GraphTraits<ValueAnalysis::Map>::NodeType *Node) {
    return !Node->second.getPtr() || Node->first != Node->second->getValue();
  }

#ifndef NDEBUG
  void viewLazyAndersenGraph(LazyAndersenData *Data) {
    ViewGraph(Data->ValueAnalyses, "LazyAndersen", false,
        "LazyAndersen analysis results");
  }
#endif

  // class LazyAndersen
  char LazyAndersen::ID = 0;

  LazyAndersen::LazyAndersen()
    : ModulePass(ID), Data(0) {
    initializeLazyAndersenPass(*PassRegistry::getPassRegistry());
  }

  bool LazyAndersen::runOnModule(Module &M) {
    assert(!Data);
    Data = Analyze::run(M);
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
}

using namespace llvm;
// TODO: What do these two bools mean?
INITIALIZE_PASS(LazyAndersen, "lazy-andersen",
                "Lazy Andersen's Algorithm for Points-To Analysis", false,
                true)
