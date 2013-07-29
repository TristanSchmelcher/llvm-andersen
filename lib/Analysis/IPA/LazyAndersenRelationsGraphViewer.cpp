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
#include "LazyAndersenGraphNode.h"
#include "LazyAndersenValueInfo.h"
#include "LazyAndersenValuePrinter.h"
#include "llvm/IR/Module.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/GraphWriter.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

namespace {
  class NodeForwardIterator {
    std::list<GraphEdge> RemainingEdges;

  public:
    typedef ssize_t difference_type;
    typedef const GraphNodeBase *value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef std::forward_iterator_tag iterator_category;

    NodeForwardIterator() {}

    explicit NodeForwardIterator(const std::list<GraphEdge> &Edges)
      : RemainingEdges(Edges) {}

    const char *getLabel() const { return RemainingEdges.front().Label; }

    value_type operator*() const { return RemainingEdges.front().Dst; }

    bool operator==(const NodeForwardIterator &that) const {
      // Since it's invalid to compare iterators from different objects, it's
      // sufficient to compare the size here.
      return RemainingEdges.size() == that.RemainingEdges.size();
    }

    bool operator!=(const NodeForwardIterator &that) const {
      return !(*this == that);
    }

    NodeForwardIterator &operator++() {
      RemainingEdges.pop_front();
      return *this;
    }

    NodeForwardIterator operator++(int) {
      NodeForwardIterator Tmp = *this;
      ++*this;
      return Tmp;
    }
  };
}

namespace llvm {
  template<>
  struct GraphTraits<LazyAndersenData> {
    typedef const GraphNodeBase NodeType;
    typedef df_iterator<LazyAndersenData> nodes_iterator;
    typedef NodeForwardIterator ChildIteratorType;

    static ChildIteratorType child_begin(NodeType *Node) {
      return ChildIteratorType(Node->getOutgoingEdges());
    }

    static ChildIteratorType child_end(NodeType *Node) {
      return ChildIteratorType();
    }

    static NodeType *getEntryNode(const LazyAndersenData &Data) {
      return &Data;
    }

    static nodes_iterator nodes_begin(const LazyAndersenData &Data) {
      return df_begin(Data);
    }

    static nodes_iterator nodes_end(const LazyAndersenData &Data) {
      return df_end(Data);
    }
  };

  template<>
  class DOTGraphTraits<LazyAndersenData> : public DefaultDOTGraphTraits {
  public:
    DOTGraphTraits(bool simple = false) : DefaultDOTGraphTraits(simple) {}

    std::string getNodeLabel(const GraphNodeBase *Node,
                             const LazyAndersenData &Data) {
      static const size_t MaxPrintedSize = 16;
      switch (Node->getType()) {
      case GraphNodeBase::VALUE_INFO:
        return prettyPrintValue(cast<ValueInfo>(Node)->getValue(),
                                MaxPrintedSize);
      default:
        llvm_unreachable("Unexpected node type");
        break;
      }
    }

    static std::string getEdgeSourceLabel(const GraphNodeBase *Node,
                                          const NodeForwardIterator &i) {
      return i.getLabel();
    }

    static bool isNodeHidden(const GraphNodeBase *Node) {
      return Node->getType() == GraphNodeBase::ROOT;
    }
  };
}

namespace llvm {
namespace lazyandersen {
  void viewRelationsGraph(const LazyAndersenData *Data, const Module *M) {
    std::ostringstream OSS;
    OSS << "LazyAndersen analysis results";
    if (M) {
      OSS << " for module " << M->getModuleIdentifier();
    }
    ViewGraph(*Data, "LazyAndersen", false, OSS.str());
  }
}
}
