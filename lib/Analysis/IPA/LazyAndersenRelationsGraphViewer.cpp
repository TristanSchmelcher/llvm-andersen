//===- LazyAndersenRelationsGraphViewer.cpp - graph viewing ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a function to display a graph of the relations.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenRelationsGraphViewer.h"

#include "LazyAndersenData.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenValueInfo.h"
#include "LazyAndersenValuePrinter.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_os_ostream.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

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

    explicit forward_iterator_adapter(const OriginalIteratorTy &i) : i(i) {}

    OriginalIteratorTy &wrappedIterator() { return i; }

    const OriginalIteratorTy &wrappedIterator() const { return i; }

    value_type operator*() const { return AdapterFunctor(*i); }

    bool operator==(const forward_iterator_adapter &that) const {
      return i == that.i;
    }

    bool operator!=(const forward_iterator_adapter &that) const {
      return i != that.i;
    }

    forward_iterator_adapter &operator++() { ++i; return *this; }

    forward_iterator_adapter operator++(int) {
      forward_iterator_adapter tmp = *this; ++*this; return tmp;
    }
  };

  class IncomingHalfRelationIteratorAdapter {
    struct AdapterFunctor {
      ValueInfo::Map::value_type *operator()(const HalfRelationBase &HR)
          const {
        ValueInfo *VI = Relation::getOppositeEndpoint(
            HalfRelation<SOURCE>::from(&HR))->getValueInfo();
        return &*VI->getMap()->find(VI->getValue());
      }
    };

  public:
    typedef forward_iterator_adapter<HalfRelationBaseList::iterator,
        ValueInfo::Map::value_type *, AdapterFunctor> iterator;
  };
}

namespace llvm {
  template<>
  struct GraphTraits<ValueInfo::Map> {
    typedef const ValueInfo::Map::value_type NodeType;
    typedef ValueInfo::Map::const_iterator nodes_iterator;
    typedef IncomingHalfRelationIteratorAdapter::iterator ChildIteratorType;

    static ChildIteratorType child_begin(NodeType *Node) {
      ValueInfo *VI = Node->second.getPtr();
      return ChildIteratorType(VI->getRelations<SOURCE>()->begin());
    }

    static ChildIteratorType child_end(NodeType *Node) {
      ValueInfo *VI = Node->second.getPtr();
      return ChildIteratorType(VI->getRelations<SOURCE>()->end());
    }

    static nodes_iterator nodes_begin(const ValueInfo::Map &Map) {
      return nodes_iterator(Map.begin());
    }

    static nodes_iterator nodes_end(const ValueInfo::Map &Map) {
      return nodes_iterator(Map.end());
    }
  };

  template<>
  class DOTGraphTraits<ValueInfo::Map> :
      public DefaultDOTGraphTraits {
  public:
    DOTGraphTraits(bool simple = false) : DefaultDOTGraphTraits(simple) {}

    std::string getNodeLabel(GraphTraits<ValueInfo::Map>::NodeType *Node,
                             const ValueInfo::Map &Map) {
      static const size_t MaxPrintedSize = 16;
      const Value *V = Node->first;
      return prettyPrintValue(V, MaxPrintedSize);
    }

    static std::string getEdgeSourceLabel(
        GraphTraits<ValueInfo::Map>::NodeType *Node,
        const GraphTraits<ValueInfo::Map>::ChildIteratorType &i) {
      return Relation::get(HalfRelation<SOURCE>::from(i.wrappedIterator()))
          ->getRelationName();
    }

    static bool isNodeHidden(GraphTraits<ValueInfo::Map>::NodeType *Node) {
      return !Node->second.getPtr() || Node->first != Node->second->getValue();
    }
  };
}

namespace llvm {
namespace lazyandersen {
  void viewRelationsGraph(LazyAndersenData *Data, const Module *M) {
    std::ostringstream OSS;
    OSS << "LazyAndersen analysis results";
    if (M) {
      OSS << " for module " << M->getModuleIdentifier();
    }
    ViewGraph(Data->ValueInfos, "LazyAndersen", false, OSS.str());
  }
}
}
