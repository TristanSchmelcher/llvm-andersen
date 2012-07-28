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
#include "llvm/ADT/OwningPtr.h"
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

    template<RelationDirection Direction>
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

    template<RelationDirection Direction> class HalfRelation;
    class HalfRelationBaseList;

    class HalfRelationBase :
        private ilist_node_with_list_ptr<HalfRelationBase> {
      friend struct ilist_nextprev_traits<HalfRelationBase>;
      friend struct ilist_node_traits<HalfRelationBase>;
      friend struct ilist_node_with_list_ptr_traits<HalfRelationBase>;
      friend struct ilist_traits<HalfRelationBase>;
      template<RelationDirection> friend class HalfRelation;

    public:
      template<RelationDirection Direction>
      const HalfRelation<Direction> *as() const;

      template<RelationDirection Direction>
      HalfRelation<Direction> *as();

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
    class ValueInfo;

    template<RelationDirection Direction>
    class HalfRelation : public HalfRelationBase,
        private direction_traits<Direction> {
      friend class Relation;

    public:
      using direction_traits<Direction>::OppositeDirection;
      typedef HalfRelation<OppositeDirection> OppositeHalfRelationTy;

      const Relation *getRelation() const;
      Relation *getRelation();
      ValueInfo *getValueInfo() const;
      const OppositeHalfRelationTy *getOppositeDirection() const;
      OppositeHalfRelationTy *getOppositeDirection();

    private:
      explicit HalfRelation(ValueInfo *VI);
      ~HalfRelation();
    };

    template<RelationDirection Direction> class HalfRelationList;

    class HalfRelationBaseList : public ilist<HalfRelationBase> {
      template<RelationDirection Direction> friend class HalfRelationList;

    public:
      template<RelationDirection Direction>
      const HalfRelationList<Direction> *as() const;

      template<RelationDirection Direction>
      HalfRelationList<Direction> *as();

    private:
      HalfRelationBaseList();
    };

    // class HalfRelationBase
    template<RelationDirection Direction>
    inline const HalfRelation<Direction> *HalfRelationBase::as() const {
      return static_cast<const HalfRelation<Direction> *>(this);
    }

    template<RelationDirection Direction>
    inline HalfRelation<Direction> *HalfRelationBase::as() {
      return static_cast<HalfRelation<Direction> *>(this);
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

    template<RelationDirection Direction>
    class HalfRelationList : public HalfRelationBaseList {
      friend class ValueInfo;

    public:
      const ValueInfo *getValueInfo() const;
      ValueInfo *getValueInfo();

    private:
      HalfRelationList();
    };

    // class HalfRelationBaseList
    template<RelationDirection Direction>
    inline const HalfRelationList<Direction> *HalfRelationBaseList::as() const {
      return static_cast<const HalfRelationList<Direction> *>(this);
    }

    template<RelationDirection Direction>
    inline HalfRelationList<Direction> *HalfRelationBaseList::as() {
      return static_cast<HalfRelationList<Direction> *>(this);
    }

    HalfRelationBaseList::HalfRelationBaseList() {}

    class LazyAnalysisStep :
        protected ilist_node_with_list_ptr<LazyAnalysisStep> {
      friend struct ilist_nextprev_traits<LazyAnalysisStep>;
      friend struct ilist_node_traits<LazyAnalysisStep>;
      friend struct ilist_node_with_list_ptr_traits<LazyAnalysisStep>;
      friend struct ilist_traits<LazyAnalysisStep>;

    public:
      // 2-tuple of next ValueInfo result and next LazyAnalysisStep to run
      // to produce the following results. One or both may be 0.
      typedef std::pair<ValueInfo *, LazyAnalysisStep *> Result;

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

    class LazyAnalysisResult;

    typedef OwningPtr<LazyAnalysisResult> LazyAnalysisResultRef;

    template<typename AlgorithmIdTy>
    struct AlgorithmGroupTraits;

#define ALGORITHM_GROUP_TRAITS(ALGORITHM_ID_TY, NUM_ALGORITHM_IDS, INPUT_TYPE) \
    template<> \
    struct AlgorithmGroupTraits<ALGORITHM_ID_TY> { \
      static const ALGORITHM_ID_TY NumAlgorithmIds = NUM_ALGORITHM_IDS; \
      typedef INPUT_TYPE InputType; \
  \
      template<ALGORITHM_ID_TY AlgorithmId> \
      struct AlgorithmTraits; \
    }

    template<typename InputTy>
    class AnalysisAlgorithm {
    public:
      virtual void getLazyResult(LazyAnalysisResult *Output, InputTy *Input)
          const = 0;
    };

    enum ValueInfoAlgorithmId {
      POINTS_TO_SET,
      REVERSE_POINTS_TO_SET,
      NUM_VALUE_INFO_ALGORITHM_IDS
    };

    ALGORITHM_GROUP_TRAITS(ValueInfoAlgorithmId,
                           NUM_VALUE_INFO_ALGORITHM_IDS,
                           ValueInfo);

    enum LazyAnalysisResultAlgorithmId {
      CONTENT_POINTS_TO_SET,
      RETURN_VALUE_POINTS_TO_SET,
      ARGUMENT_POINTS_TO_SET,
      // TODO
      NUM_LAZY_ANALYSIS_RESULT_ALGORITHM_IDS
    };

    ALGORITHM_GROUP_TRAITS(LazyAnalysisResultAlgorithmId,
                           NUM_LAZY_ANALYSIS_RESULT_ALGORITHM_IDS,
                           LazyAnalysisResult);

#define ALGORITHM_TRAITS(ALGORITHM_ID, ALGORITHM_TYPE) \
    template<> \
    struct AlgorithmGroupTraits<typeof ALGORITHM_ID>::AlgorithmTraits< \
        ALGORITHM_ID> { \
      typedef ALGORITHM_TYPE Type; \
    }

    template<typename AlgorithmIdTy>
    class AlgorithmResultCache : private AlgorithmGroupTraits<AlgorithmIdTy> {
      using AlgorithmGroupTraits<AlgorithmIdTy>::NumAlgorithmIds;
      typedef typename AlgorithmGroupTraits<AlgorithmIdTy>::InputType InputType;

      LazyAnalysisResultRef Results[NumAlgorithmIds];

    public:
      template<AlgorithmIdTy AlgorithmId>
      LazyAnalysisResult *getAlgorithmResult();

    private:
      LazyAnalysisResult *getAlgorithmResultInternal(AlgorithmIdTy AlgorithmId,
          const AnalysisAlgorithm<InputType> &Algorithm, InputType *Input);
    };

    class LazyAnalysisResult : public LazyAnalysisStepList,
        public AlgorithmResultCache<LazyAnalysisResultAlgorithmId> {
    public:
      typedef LazyAnalysisResultRef Ref;

      ~LazyAnalysisResult();
    };

    // class LazyAnalysisResult
    LazyAnalysisResult::~LazyAnalysisResult() {}

    // class AlgorithmResultCache
    template<typename AlgorithmIdTy>
    template<AlgorithmIdTy AlgorithmId>
    LazyAnalysisResult *
    AlgorithmResultCache<AlgorithmIdTy>::getAlgorithmResult() {
      return getAlgorithmResultInternal(AlgorithmId,
          typename AlgorithmGroupTraits<AlgorithmIdTy>
              ::template AlgorithmTraits<AlgorithmId>::Type(),
          static_cast<InputType *>(this));
    }

    template<typename AlgorithmIdTy>
    LazyAnalysisResult *
    AlgorithmResultCache<AlgorithmIdTy>::getAlgorithmResultInternal(
        AlgorithmIdTy AlgorithmId,
        const AnalysisAlgorithm<InputType> &Algorithm, InputType *Input) {
      LazyAnalysisResult *Result = Results[AlgorithmId].get();
      if (!Result) {
        Result = new LazyAnalysisResult();
        Algorithm.getLazyResult(Result, Input);
        Results[AlgorithmId].reset(Result);
      }
      return Result;
    }

    // A Relation is a directed edge in the analysis graph.
    class Relation :
        private HalfRelation<INCOMING>,
        private HalfRelation<OUTGOING> {
      template<RelationDirection Direction> friend class HalfRelation;

    public:
      Relation(ValueInfo *Src, ValueInfo *Dst);
      virtual ~Relation();

      template<RelationDirection Direction>
      const HalfRelation<Direction> *getDirection() const;

      template<RelationDirection Direction>
      HalfRelation<Direction> *getDirection();

      template<RelationDirection Direction>
      ValueInfo *getValueInfo() const;

      virtual const char *getRelationName() const = 0;
      virtual LazyAnalysisResult *analyzePointsToSet() const = 0;
    };

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

    class RecursiveAnalysisStep : public LazyAnalysisStep {
      // We assume here that--once created--each LazyAnalysisResult will persist
      // for the lifetime of the LazyAndersenData. If not, we'll need weak
      // pointers.
      LazyAnalysisResult *Inner;

    public:
      RecursiveAnalysisStep(LazyAnalysisResult *Inner);
      // TODO: Need a way to track which inner member is next. Perhaps run
      // needs to take a context? Or we could have a stack.
      virtual Result run();
    };

    // class RecursiveAnalysisStep
    RecursiveAnalysisStep::RecursiveAnalysisStep(LazyAnalysisResult *Inner)
      : Inner(Inner) {}

    RecursiveAnalysisStep::Result RecursiveAnalysisStep::run() {
      // TODO
      return RecursiveAnalysisStep::Result();
    }

    // Helper class for analyzing relations in a particular direction.
    class RelationsAnalysisStepBase : public IterativeAnalysisStep {
      HalfRelationBaseList *List;
      HalfRelationBaseList::iterator i;

    public:
      explicit RelationsAnalysisStepBase(HalfRelationBaseList *List);
      virtual Result run();

    private:
      virtual LazyAnalysisResult *analyzeHalfRelation(HalfRelationBase *HR) = 0;
    };

    // class RelationsAnalysisStepBase
    RelationsAnalysisStepBase::RelationsAnalysisStepBase(
        HalfRelationBaseList *List)
      : List(List), i(List->begin()) {}

    RelationsAnalysisStepBase::Result RelationsAnalysisStepBase::run() {
      while (i != List->end()) {
        HalfRelationBase *HR = &*i;
        ++i;
        LazyAnalysisResult *LR = analyzeHalfRelation(HR);
        if (LR) {
          return nextIteration(new RecursiveAnalysisStep(LR));
        }
        // Else nothing to do for this relation; keep going.
      }
      // No relations left to analyze.
      return done();
    }

    template<RelationDirection Direction, typename StepTy>
    class RelationsAnalysisStep : public RelationsAnalysisStepBase {
    public:
      explicit RelationsAnalysisStep(ValueInfo *VI);

    private:
      virtual LazyAnalysisResult *analyzeHalfRelation(HalfRelationBase *HR);
    };

    class PointsToRelationsAnalysisStep : public RelationsAnalysisStep<OUTGOING,
        PointsToRelationsAnalysisStep> {
      friend class RelationsAnalysisStep<OUTGOING,
          PointsToRelationsAnalysisStep>;

    public:
      explicit PointsToRelationsAnalysisStep(ValueInfo *VI);

    private:
      LazyAnalysisResult *analyzeRelation(Relation *R);
    };

    // class PointsToRelationsAnalysisStep
    inline PointsToRelationsAnalysisStep::PointsToRelationsAnalysisStep(
        ValueInfo *VI)
      : RelationsAnalysisStep<OUTGOING, PointsToRelationsAnalysisStep>(VI) {}

    inline LazyAnalysisResult *PointsToRelationsAnalysisStep::analyzeRelation(
        Relation *R) {
      return R->analyzePointsToSet();
    }

    class PointsToAlgorithm : public AnalysisAlgorithm<ValueInfo> {
    public:
      virtual void getLazyResult(LazyAnalysisResult *Output, ValueInfo *Input)
          const;
    };

    ALGORITHM_TRAITS(POINTS_TO_SET, PointsToAlgorithm);

    // class PointsToAlgorithm
    void PointsToAlgorithm::getLazyResult(LazyAnalysisResult *Output,
        ValueInfo *Input) const {
      Output->push_back(new PointsToRelationsAnalysisStep(Input));
    }

    class ReversePointsToOutgoingRelationsAnalysisStep :
        public RelationsAnalysisStep<OUTGOING,
            ReversePointsToOutgoingRelationsAnalysisStep> {
      friend class RelationsAnalysisStep<OUTGOING,
          ReversePointsToOutgoingRelationsAnalysisStep>;

    public:
      explicit ReversePointsToOutgoingRelationsAnalysisStep(ValueInfo *VI);

    private:
      LazyAnalysisResult *analyzeRelation(Relation *R);
    };

    // class ReversePointsToOutgoingRelationsAnalysisStep
    inline ReversePointsToOutgoingRelationsAnalysisStep
        ::ReversePointsToOutgoingRelationsAnalysisStep(ValueInfo *VI)
      : RelationsAnalysisStep<OUTGOING,
            ReversePointsToOutgoingRelationsAnalysisStep>(VI) {}

    inline LazyAnalysisResult *
    ReversePointsToOutgoingRelationsAnalysisStep::analyzeRelation(Relation *R) {
      // TODO
      return 0;
    }

    class ReversePointsToIncomingRelationsAnalysisStep :
        public RelationsAnalysisStep<INCOMING,
            ReversePointsToIncomingRelationsAnalysisStep> {
      friend class RelationsAnalysisStep<INCOMING,
          ReversePointsToIncomingRelationsAnalysisStep>;

    public:
      explicit ReversePointsToIncomingRelationsAnalysisStep(ValueInfo *VI);

    private:
      LazyAnalysisResult *analyzeRelation(Relation *R);
    };

    // class ReversePointsToIncomingRelationsAnalysisStep
    inline ReversePointsToIncomingRelationsAnalysisStep
        ::ReversePointsToIncomingRelationsAnalysisStep(ValueInfo *VI)
      : RelationsAnalysisStep<INCOMING,
            ReversePointsToIncomingRelationsAnalysisStep>(VI) {}

    inline LazyAnalysisResult *
    ReversePointsToIncomingRelationsAnalysisStep::analyzeRelation(Relation *R) {
      // TODO
      return 0;
    }

    class ReversePointsToAlgorithm : public AnalysisAlgorithm<ValueInfo> {
    public:
      virtual void getLazyResult(LazyAnalysisResult *Output, ValueInfo *Input)
          const;
    };

    ALGORITHM_TRAITS(REVERSE_POINTS_TO_SET, ReversePointsToAlgorithm);

    // class ReversePointsToAlgorithm
    void ReversePointsToAlgorithm::getLazyResult(LazyAnalysisResult *Output,
        ValueInfo *Input) const {
      Output->push_back(new ReversePointsToOutgoingRelationsAnalysisStep(
          Input));
      Output->push_back(new ReversePointsToIncomingRelationsAnalysisStep(
          Input));
    }

    class ContentPointsToAlgorithm :
        public AnalysisAlgorithm<LazyAnalysisResult> {
    public:
      virtual void getLazyResult(LazyAnalysisResult *Output,
          LazyAnalysisResult *Input) const;
    };

    ALGORITHM_TRAITS(CONTENT_POINTS_TO_SET, ContentPointsToAlgorithm);

    // class ContentPointsToAlgorithm
    void ContentPointsToAlgorithm::getLazyResult(LazyAnalysisResult *Output,
        LazyAnalysisResult *Input) const {
      // TODO
    }

    class ReturnValuePointsToAlgorithm :
        public AnalysisAlgorithm<LazyAnalysisResult> {
    public:
      virtual void getLazyResult(LazyAnalysisResult *Output,
          LazyAnalysisResult *Input) const;
    };

    ALGORITHM_TRAITS(RETURN_VALUE_POINTS_TO_SET, ReturnValuePointsToAlgorithm);

    // class ReturnValuePointsToAlgorithm
    void ReturnValuePointsToAlgorithm::getLazyResult(LazyAnalysisResult *Output,
        LazyAnalysisResult *Input) const {
      // TODO
    }

    class ArgumentPointsToAlgorithm :
        public AnalysisAlgorithm<LazyAnalysisResult> {
    public:
      virtual void getLazyResult(LazyAnalysisResult *Output,
          LazyAnalysisResult *Input) const;
    };

    ALGORITHM_TRAITS(ARGUMENT_POINTS_TO_SET, ArgumentPointsToAlgorithm);

    // class ArgumentPointsToAlgorithm
    void ArgumentPointsToAlgorithm::getLazyResult(LazyAnalysisResult *Output,
        LazyAnalysisResult *Input) const {
      // TODO
    }

    class ValueInfo : private RefCountedBase<ValueInfo>,
        private HalfRelationList<INCOMING>,
        private HalfRelationList<OUTGOING>,
        public AlgorithmResultCache<ValueInfoAlgorithmId> {
      friend struct IntrusiveRefCntPtrInfo<ValueInfo>;
      friend class RefCountedBase<ValueInfo>;
      template<RelationDirection Direction> friend class HalfRelationList;
      // The Value that maps to this object. (If this analysis applies to
      // multiple Values, this is the first one that was analyzed.)
      const Value *V;

    public:
      typedef IntrusiveRefCntPtr<ValueInfo> Ref;
      // TODO: Should this be a ValueMap?
      typedef DenseMap<const Value *, Ref> Map;

    private:
      // The map that this analysis is in.
      Map *ContainingMap;

    public:
      static ValueInfo *const Nil;

      ValueInfo(const Value *V, Map *Map);
      const Value *getValue() const;
      Map *getMap() const;

      template<RelationDirection Direction>
      const HalfRelationList<Direction> *getRelations() const;

      template<RelationDirection Direction>
      HalfRelationList<Direction> *getRelations();

    private:
      ~ValueInfo();
    };

    // class ValueInfo
    ValueInfo *const ValueInfo::Nil = 0;

    ValueInfo::ValueInfo(const Value *V, Map *ContainingMap)
      : V(V), ContainingMap(ContainingMap) {}

    inline const Value *ValueInfo::getValue() const {
      return V;
    }

    inline ValueInfo::Map *ValueInfo::getMap() const {
      return ContainingMap;
    }

    template<RelationDirection Direction>
    inline const HalfRelationList<Direction> *ValueInfo::getRelations()
        const {
      return static_cast<const HalfRelationList<Direction> *>(this);
    }

    template<RelationDirection Direction>
    inline HalfRelationList<Direction> *ValueInfo::getRelations() {
      return static_cast<HalfRelationList<Direction> *>(this);
    }

    ValueInfo::~ValueInfo() {}

    // class HalfRelation
    template<RelationDirection Direction>
    inline const Relation *HalfRelation<Direction>::getRelation() const {
      return static_cast<const Relation *>(this);
    }

    template<RelationDirection Direction>
    inline Relation *HalfRelation<Direction>::getRelation() {
      return static_cast<Relation *>(this);
    }

    template<RelationDirection Direction>
    ValueInfo *HalfRelation<Direction>::getValueInfo() const {
      assert(getList());
      return getList()->as<Direction>()->getValueInfo();
    }

    template<RelationDirection Direction>
    const typename HalfRelation<Direction>::OppositeHalfRelationTy *
    HalfRelation<Direction>::getOppositeDirection() const {
      return getRelation()->template getDirection<OppositeDirection>();
    }

    template<RelationDirection Direction>
    typename HalfRelation<Direction>::OppositeHalfRelationTy *
    HalfRelation<Direction>::getOppositeDirection() {
      return getRelation()->template getDirection<OppositeDirection>();
    }

    template<RelationDirection Direction>
    inline HalfRelation<Direction>::HalfRelation(ValueInfo *VI)
      : HalfRelationBase(VI->getRelations<Direction>()) {}

    template<RelationDirection Direction>
    HalfRelation<Direction>::~HalfRelation() {}

    // class HalfRelationList
    template<RelationDirection Direction>
    inline const ValueInfo *HalfRelationList<Direction>::getValueInfo()
        const {
      return static_cast<const ValueInfo *>(this);
    }

    template<RelationDirection Direction>
    inline ValueInfo *HalfRelationList<Direction>::getValueInfo() {
      return static_cast<ValueInfo *>(this);
    }

    template<RelationDirection Direction>
    HalfRelationList<Direction>::HalfRelationList() {}

    // class Relation
    Relation::Relation(ValueInfo *Src, ValueInfo *Dst)
      : HalfRelation<INCOMING>(Src), HalfRelation<OUTGOING>(Dst) {}

    Relation::~Relation() {}

    template<RelationDirection Direction>
    inline const HalfRelation<Direction> *Relation::getDirection() const {
      return static_cast<const HalfRelation<Direction> *>(this);
    }

    template<RelationDirection Direction>
    inline HalfRelation<Direction> *Relation::getDirection() {
      return static_cast<HalfRelation<Direction> *>(this);
    }

    template<RelationDirection Direction>
    inline ValueInfo *Relation::getValueInfo() const {
      return getDirection<Direction>()->getValueInfo();
    }

    // class RelationsAnalysisStep
    template<RelationDirection Direction, typename StepTy>
    RelationsAnalysisStep<Direction, StepTy>::RelationsAnalysisStep(
        ValueInfo *VI)
      : RelationsAnalysisStepBase(VI->getRelations<Direction>()) {}

    template<RelationDirection Direction, typename StepTy>
    LazyAnalysisResult *RelationsAnalysisStep<Direction, StepTy>::
        analyzeHalfRelation(HalfRelationBase *HR) {
      return static_cast<StepTy *>(this)->analyzeRelation(
          HR->as<Direction>()->getRelation());
    }
  }

  class LazyAndersenData {
  public:
    ValueInfo::Map ValueInfos;
  };

  namespace {
    class DependsOnRelation : public Relation {
    public:
      DependsOnRelation(ValueInfo *DependentValueInfo,
          ValueInfo *IndependentValueInfo);
      virtual const char *getRelationName() const;
      virtual LazyAnalysisResult *analyzePointsToSet() const;
    };

    // class DependsOnRelation
    inline DependsOnRelation::DependsOnRelation(
        ValueInfo *DependentValueInfo,
        ValueInfo *IndependentValueInfo)
      : Relation(DependentValueInfo, IndependentValueInfo) {}

    const char *DependsOnRelation::getRelationName() const {
      return "depends on";
    }

    LazyAnalysisResult *DependsOnRelation::analyzePointsToSet() const {
      return getValueInfo<INCOMING>()->getAlgorithmResult<POINTS_TO_SET>();
    }

    class LoadedFromRelation : public Relation {
    public:
      LoadedFromRelation(ValueInfo *LoadedValueInfo,
          ValueInfo *AddressValueInfo);
      virtual const char *getRelationName() const;
      virtual LazyAnalysisResult *analyzePointsToSet() const;
    };

    // class LoadedFromRelation
    inline LoadedFromRelation::LoadedFromRelation(
        ValueInfo *LoadedValueInfo, ValueInfo *AddressValueInfo)
      : Relation(LoadedValueInfo, AddressValueInfo) {}

    const char *LoadedFromRelation::getRelationName() const {
      return "loaded from";
    }

    LazyAnalysisResult *LoadedFromRelation::analyzePointsToSet() const {
      return getValueInfo<INCOMING>()->getAlgorithmResult<POINTS_TO_SET>()
          ->getAlgorithmResult<CONTENT_POINTS_TO_SET>();
    }

    class StoredToRelation : public Relation {
    public:
      StoredToRelation(ValueInfo *StoredValueInfo,
          ValueInfo *AddressValueInfo);
      virtual const char *getRelationName() const;
      virtual LazyAnalysisResult *analyzePointsToSet() const;
    };

    // class StoredToRelation
    inline StoredToRelation::StoredToRelation(ValueInfo *StoredValueInfo,
        ValueInfo *AddressValueInfo)
      : Relation(StoredValueInfo, AddressValueInfo) {}

    const char *StoredToRelation::getRelationName() const {
      return "stored to";
    }

    LazyAnalysisResult *StoredToRelation::analyzePointsToSet() const {
      // No effect on points-to set.
      return 0;
    }

    class ReturnedFromCalleeRelation : public Relation {
    public:
      ReturnedFromCalleeRelation(ValueInfo *ReturnedValueInfo,
          ValueInfo *CalledValueInfo);
      virtual const char *getRelationName() const;
      virtual LazyAnalysisResult *analyzePointsToSet() const;
    };

    // class ReturnedFromCalleeRelation
    inline ReturnedFromCalleeRelation::ReturnedFromCalleeRelation(
        ValueInfo *ReturnedValueInfo, ValueInfo *CalledValueInfo)
      : Relation(ReturnedValueInfo, CalledValueInfo) {}

    const char *ReturnedFromCalleeRelation::getRelationName() const {
      return "returned from callee";
    }

    LazyAnalysisResult *ReturnedFromCalleeRelation::analyzePointsToSet() const {
      return getValueInfo<INCOMING>()->getAlgorithmResult<POINTS_TO_SET>()
          ->getAlgorithmResult<RETURN_VALUE_POINTS_TO_SET>();
    }

    class ArgumentToCalleeRelation : public Relation {
    public:
      ArgumentToCalleeRelation(ValueInfo *ArgumentValueInfo,
          ValueInfo *CalledValueInfo);
      virtual const char *getRelationName() const;
      virtual LazyAnalysisResult *analyzePointsToSet() const;
    };

    // class ArgumentToCalleeRelation
    inline ArgumentToCalleeRelation::ArgumentToCalleeRelation(
        ValueInfo *ArgumentValueInfo, ValueInfo *CalledValueInfo)
      : Relation(ArgumentValueInfo, CalledValueInfo) {}

    const char *ArgumentToCalleeRelation::getRelationName() const {
      return "argument to callee";
    }

    LazyAnalysisResult *ArgumentToCalleeRelation::analyzePointsToSet() const {
      // No effect on points-to set.
      return 0;
    }

    class ArgumentFromCallerRelation : public Relation {
    public:
      ArgumentFromCallerRelation(ValueInfo *ArgumentValueInfo,
          ValueInfo *FunctionValueInfo);
      virtual const char *getRelationName() const;
      virtual LazyAnalysisResult *analyzePointsToSet() const;
    };

    // class ArgumentFromCallerRelation
    inline ArgumentFromCallerRelation::ArgumentFromCallerRelation(
        ValueInfo *ArgumentValueInfo,
        ValueInfo *FunctionValueInfo)
      : Relation(ArgumentValueInfo, FunctionValueInfo) {}

    const char *ArgumentFromCallerRelation::getRelationName() const {
      return "argument from caller";
    }

    LazyAnalysisResult *ArgumentFromCallerRelation::analyzePointsToSet() const {
      return getValueInfo<INCOMING>()
          ->getAlgorithmResult<REVERSE_POINTS_TO_SET>()
              ->getAlgorithmResult<ARGUMENT_POINTS_TO_SET>();
    }

    class ReturnedToCallerRelation : public Relation {
    public:
      ReturnedToCallerRelation(ValueInfo *ReturnedValueInfo,
          ValueInfo *FunctionValueInfo);
      virtual const char *getRelationName() const;
      virtual LazyAnalysisResult *analyzePointsToSet() const;
    };

    // class ReturnedToCallerRelation
    inline ReturnedToCallerRelation::ReturnedToCallerRelation(
        ValueInfo *ReturnedValueInfo,
        ValueInfo *FunctionValueInfo)
      : Relation(ReturnedValueInfo, FunctionValueInfo) {}

    const char *ReturnedToCallerRelation::getRelationName() const {
      return "returned to caller";
    }

    LazyAnalysisResult *ReturnedToCallerRelation::analyzePointsToSet() const {
      // No effect on points-to set.
      return 0;
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

    // class InstructionAnalyzer
    LazyAndersenData *InstructionAnalyzer::run(Module &M) {
      return InstructionAnalyzer(M).Data;
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

    InstructionAnalyzer::InstructionAnalyzer(Module &M)
      : Data(new LazyAndersenData()) {
      for (Module::iterator i = M.begin(); i != M.end(); ++i) {
        Function &F(*i);
        CurrentFunction = &F;
        visit(F);
        processPHINodes();
      }
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

    void InstructionAnalyzer::processPHINodes() {
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
      // TODO: Pass finalized points-to set as argument and pre-create the set.
      return createValueInfo(V);
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
      // to something else.
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

    class ValueInfoMapIteratorAdapter {
      struct AdapterFunctor {
        const ValueInfo::Map::value_type *operator()(
            const ValueInfo::Map::value_type &value) const;
      };

    public:
      typedef forward_iterator_adapter<ValueInfo::Map::const_iterator,
          const ValueInfo::Map::value_type *, AdapterFunctor> iterator;
    };

    // class ValueInfoMapIteratorAdapter::AdapterFunctor
    inline const ValueInfo::Map::value_type *
    ValueInfoMapIteratorAdapter::AdapterFunctor::operator()(
        const ValueInfo::Map::value_type &value) const {
      return &value;
    }

    class IncomingHalfRelationIteratorAdapter {
      struct AdapterFunctor {
        ValueInfo::Map::value_type *operator()(const HalfRelationBase &HR)
            const;
      };

    public:
      typedef forward_iterator_adapter<HalfRelationBaseList::iterator,
          ValueInfo::Map::value_type *, AdapterFunctor> iterator;
    };

    // class IncomingHalfRelationIteratorAdapter::AdapterFunctor
    ValueInfo::Map::value_type *IncomingHalfRelationIteratorAdapter::
        AdapterFunctor::operator()(const HalfRelationBase &HR) const {
      ValueInfo *VI = HR.as<INCOMING>()->getOppositeDirection()
          ->getValueInfo();
      return &*VI->getMap()->find(VI->getValue());
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
  struct GraphTraits<ValueInfo::Map> {
    typedef const ValueInfo::Map::value_type NodeType;
    typedef ValueInfoMapIteratorAdapter::iterator nodes_iterator;
    typedef IncomingHalfRelationIteratorAdapter::iterator ChildIteratorType;

    static ChildIteratorType child_begin(NodeType *Node);
    static ChildIteratorType child_end(NodeType *Node);
    static nodes_iterator nodes_begin(const ValueInfo::Map &Map);
    static nodes_iterator nodes_end(const ValueInfo::Map &Map);
  };

  // struct GraphTraits
  GraphTraits<ValueInfo::Map>::ChildIteratorType
  GraphTraits<ValueInfo::Map>::child_begin(
      NodeType *Node) {
    ValueInfo *VI = Node->second.getPtr();
    return ChildIteratorType(VI->getRelations<INCOMING>()->begin());
  }

  GraphTraits<ValueInfo::Map>::ChildIteratorType
  GraphTraits<ValueInfo::Map>::child_end(NodeType *Node) {
    ValueInfo *VI = Node->second.getPtr();
    return ChildIteratorType(VI->getRelations<INCOMING>()->end());
  }

  GraphTraits<ValueInfo::Map>::nodes_iterator
  GraphTraits<ValueInfo::Map>::nodes_begin(
      const ValueInfo::Map &Map) {
    return nodes_iterator(Map.begin());
  }

  GraphTraits<ValueInfo::Map>::nodes_iterator
  GraphTraits<ValueInfo::Map>::nodes_end(
      const ValueInfo::Map &Map) {
    return nodes_iterator(Map.end());
  }

  template<>
  class DOTGraphTraits<ValueInfo::Map> : public DefaultDOTGraphTraits {
  public:
    DOTGraphTraits(bool simple = false);
    std::string getNodeLabel(GraphTraits<ValueInfo::Map>::NodeType *Node,
        const ValueInfo::Map &Map);
    static std::string getEdgeSourceLabel(
        GraphTraits<ValueInfo::Map>::NodeType *Node,
        const GraphTraits<ValueInfo::Map>::ChildIteratorType &i);
    static bool isNodeHidden(GraphTraits<ValueInfo::Map>::NodeType *Node);
  };

  // class DOTGraphTraits
  DOTGraphTraits<ValueInfo::Map>::DOTGraphTraits(bool simple)
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

  std::string DOTGraphTraits<ValueInfo::Map>::getNodeLabel(
      GraphTraits<ValueInfo::Map>::NodeType *Node,
      const ValueInfo::Map &Map) {
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

  std::string DOTGraphTraits<ValueInfo::Map>::getEdgeSourceLabel(
      GraphTraits<ValueInfo::Map>::NodeType *Node,
      const GraphTraits<ValueInfo::Map>::ChildIteratorType &i) {
    return i.wrappedIterator()->as<INCOMING>()->getRelation()
        ->getRelationName();
  }

  bool DOTGraphTraits<ValueInfo::Map>::isNodeHidden(
      GraphTraits<ValueInfo::Map>::NodeType *Node) {
    return !Node->second.getPtr() || Node->first != Node->second->getValue();
  }

#ifndef NDEBUG
  void viewLazyAndersenGraph(LazyAndersenData *Data) {
    ViewGraph(Data->ValueInfos, "LazyAndersen", false,
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
    Data = InstructionAnalyzer::run(M);
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
    WriteGraph(OS, Data->ValueInfos, false,
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
