//===- LazyAndersenGraphViewer.cpp - graph viewing ------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines passes to display or write a graph of the andersen data
// structures.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenData.h"
#include "LazyAndersenGraphNode.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/LazyAndersen.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/GraphWriter.h"

#include <sstream>
#include <string>

using namespace llvm;
using namespace llvm::andersen_internal;

namespace llvm {

namespace {

class NodeForwardIterator {
  GraphEdgeDeque RemainingEdges;

public:
  typedef ssize_t difference_type;
  typedef const GraphNode *value_type;
  typedef value_type *pointer;
  typedef value_type &reference;
  typedef std::forward_iterator_tag iterator_category;

  NodeForwardIterator() {}

  explicit NodeForwardIterator(const GraphEdgeDeque &Edges)
    : RemainingEdges(Edges) {}

  const std::string &getLabel() const { return RemainingEdges.front().Label; }

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

template<>
struct GraphTraits<LazyAndersenData> {
  typedef const GraphNode NodeType;
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

  std::string getNodeLabel(const GraphNode *Node,
                           const LazyAndersenData &Data) {
    return Node->getNodeLabel(Data);
  }

  static std::string getEdgeSourceLabel(const GraphNode *Node,
                                        const NodeForwardIterator &i) {
    return i.getLabel();
  }

  static bool isNodeHidden(const GraphNode *Node) {
    return Node->isNodeHidden();
  }
};

}

namespace llvm {
namespace andersen_internal {

namespace {

std::string getGraphTitle(const Module *M) {
  std::ostringstream OSS;
  OSS << "LazyAndersen analysis results";
  if (M) {
    OSS << " for module " << M->getModuleIdentifier();
  }
  return OSS.str();
}

}

void viewGraph(const LazyAndersenData *Data, const Module *M) {
  ViewGraph(*Data, "LazyAndersen", false, getGraphTitle(M));
}

void printGraph(const LazyAndersenData *Data, const Module *M) {
  const char *Filename = "andersen.dot";
  errs() << "Writing '" << Filename << "'...";

  std::string ErrorInfo;
  raw_fd_ostream File(Filename, ErrorInfo);

  if (ErrorInfo.empty())
    WriteGraph(File, *Data, false, getGraphTitle(M));
  else
    errs() << "  error opening file for writing!";
  errs() << "\n";
}

class LazyAndersenGraphPass : public ModulePass {
public:
  explicit LazyAndersenGraphPass(char &ID) : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) {
    process(getAnalysis<LazyAndersen>().Data, M);
    return false;
  }

  virtual void print(raw_ostream &OS, const Module* = 0) const {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<LazyAndersen>();
  }

protected:
  virtual void process(const LazyAndersenData *Data, const Module &M) = 0;
};

namespace {

class LazyAndersenGraphViewer : public LazyAndersenGraphPass {
public:
  static char ID; // Pass identifcation, replacement for typeid

  LazyAndersenGraphViewer() : LazyAndersenGraphPass(ID) {
    initializeLazyAndersenGraphViewerPass(*PassRegistry::getPassRegistry());
  }

protected:
  virtual void process(const LazyAndersenData *Data, const Module &M) {
    viewGraph(Data, &M);
  }
};

class LazyAndersenGraphPrinter : public LazyAndersenGraphPass {
public:
  static char ID; // Pass identifcation, replacement for typeid

  LazyAndersenGraphPrinter() : LazyAndersenGraphPass(ID) {
    initializeLazyAndersenGraphPrinterPass(*PassRegistry::getPassRegistry());
  }

protected:
  virtual void process(const LazyAndersenData *Data, const Module &M) {
    printGraph(Data, &M);
  }
};

}

}
}

char LazyAndersenGraphViewer::ID = 0;
INITIALIZE_PASS_BEGIN(LazyAndersenGraphViewer, "view-andersen",
                      "View Andersen graph", false, true)
INITIALIZE_PASS_DEPENDENCY(LazyAndersen)
INITIALIZE_PASS_END(LazyAndersenGraphViewer, "view-andersen",
                    "View Andersen graph", false, true)

char LazyAndersenGraphPrinter::ID = 0;
INITIALIZE_PASS_BEGIN(LazyAndersenGraphPrinter, "dot-andersen",
                      "Print Andersen graph to 'dot' file", false, true)
INITIALIZE_PASS_DEPENDENCY(LazyAndersen)
INITIALIZE_PASS_END(LazyAndersenGraphPrinter, "dot-andersen",
                    "Print Andersen graph to 'dot' file", false, true)
