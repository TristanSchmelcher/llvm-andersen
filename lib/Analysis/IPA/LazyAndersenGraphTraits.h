//===- LazyAndersenGraphTraits.h - graph traits ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares graph traits specializations for graph printing.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENGRAPHTRAITS_H
#define LAZYANDERSENGRAPHTRAITS_H

#include "LazyAndersenValueInfo.h"
#include "llvm/Support/GraphWriter.h"

namespace llvm {
  class LazyAndersenData;
  class Module;
}

namespace llvm {
namespace lazyandersen {
  class HalfRelationBase;

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
}
}

namespace llvm {
  template<>
  struct GraphTraits<lazyandersen::ValueInfo::Map> {
    typedef const lazyandersen::ValueInfo::Map::value_type NodeType;
    typedef lazyandersen::ValueInfoMapIteratorAdapter::iterator nodes_iterator;
    typedef lazyandersen::IncomingHalfRelationIteratorAdapter::iterator
        ChildIteratorType;

    static ChildIteratorType child_begin(NodeType *Node);
    static ChildIteratorType child_end(NodeType *Node);
    static nodes_iterator nodes_begin(const lazyandersen::ValueInfo::Map &Map);
    static nodes_iterator nodes_end(const lazyandersen::ValueInfo::Map &Map);
  };

  template<>
  class DOTGraphTraits<lazyandersen::ValueInfo::Map> :
      public DefaultDOTGraphTraits {
  public:
    DOTGraphTraits(bool simple = false);
    std::string getNodeLabel(
        GraphTraits<lazyandersen::ValueInfo::Map>::NodeType *Node,
        const lazyandersen::ValueInfo::Map &Map);
    static std::string getEdgeSourceLabel(
        GraphTraits<lazyandersen::ValueInfo::Map>::NodeType *Node,
        const GraphTraits<lazyandersen::ValueInfo::Map>::ChildIteratorType &i);
    static bool isNodeHidden(
        GraphTraits<lazyandersen::ValueInfo::Map>::NodeType *Node);
  };
}

namespace llvm {
namespace lazyandersen {
void viewLazyAndersenGraph(LazyAndersenData *Data, const Module *M = 0);
}
}

#endif
