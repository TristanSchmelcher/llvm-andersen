//===- AndersenGraphViewer.cpp - graph viewing ----------------------------===//
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

#include "Data.h"
#include "DebugInfo.h"
#include "GraphNode.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/AndersenPass.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/GraphWriter.h"
#include "llvm/Support/raw_os_ostream.h"

#include <sstream>
#include <string>

namespace llvm {

using namespace andersen_internal;

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

std::string getGraphTitle(const Module *M) {
  std::ostringstream OSS;
  OSS << "AndersenPass analysis results";
  if (M) {
    OSS << " for module " << M->getModuleIdentifier();
  }
  return OSS.str();
}

}

template<>
struct GraphTraits<DebugInfo> {
  typedef const GraphNode NodeType;
  typedef df_iterator<DebugInfo> nodes_iterator;
  typedef NodeForwardIterator ChildIteratorType;

  static ChildIteratorType child_begin(NodeType *Node) {
    return ChildIteratorType(Node->getOutgoingEdges());
  }

  static ChildIteratorType child_end(NodeType *Node) {
    return ChildIteratorType();
  }

  static NodeType *getEntryNode(const DebugInfo &DI) {
    return DI.getData();
  }

  static nodes_iterator nodes_begin(const DebugInfo &DI) {
    return df_begin(DI);
  }

  static nodes_iterator nodes_end(const DebugInfo &DI) {
    return df_end(DI);
  }
};

template<>
class DOTGraphTraits<DebugInfo> : public DefaultDOTGraphTraits {
public:
  DOTGraphTraits(bool simple = false) : DefaultDOTGraphTraits(simple) {}

  std::string getNodeLabel(const GraphNode *Node, const DebugInfo &DI) {
    std::ostringstream OSS;
    {
      raw_os_ostream OS(OSS);
      Node->printNodeLabel(DI, OS);
    }
    return OSS.str();
  }

  static std::string getEdgeSourceLabel(const GraphNode *Node,
                                        const NodeForwardIterator &i) {
    return i.getLabel();
  }

  static bool isNodeHidden(const GraphNode *Node) {
    return Node->isNodeHidden();
  }
};

void viewGraph(const Data *Data, const Module *M) {
  ViewGraph(DebugInfo(Data), "AndersenPass", false, getGraphTitle(M));
}

void printGraph(const Data *Data, const Module *M) {
  const char *Filename = "andersen.dot";
  errs() << "Writing '" << Filename << "'...";

  std::string ErrorInfo;
  raw_fd_ostream File(Filename, ErrorInfo);

  if (ErrorInfo.empty()) {
    WriteGraph(File, DebugInfo(Data), false, getGraphTitle(M));
  } else {
    errs() << "  error opening file for writing!";
  }
  errs() << "\n";
}

class AndersenGraphPass : public ModulePass {
public:
  explicit AndersenGraphPass(char &ID) : ModulePass(ID) {}

  virtual bool runOnModule(Module &M) {
    process(getAnalysis<AndersenPass>().Data, M);
    return false;
  }

  virtual void print(raw_ostream &OS, const Module* = 0) const {}

  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
    AU.addRequired<AndersenPass>();
  }

protected:
  virtual void process(const Data *Data, const Module &M) = 0;
};

namespace {

class AndersenGraphViewer : public AndersenGraphPass {
public:
  static char ID; // Pass identification, replacement for typeid

  AndersenGraphViewer() : AndersenGraphPass(ID) {
    initializeAndersenGraphViewerPass(*PassRegistry::getPassRegistry());
  }

protected:
  virtual void process(const Data *Data, const Module &M) {
    viewGraph(Data, &M);
  }
};

char AndersenGraphViewer::ID = 0;

class AndersenGraphPrinter : public AndersenGraphPass {
public:
  static char ID; // Pass identification, replacement for typeid

  AndersenGraphPrinter() : AndersenGraphPass(ID) {
    initializeAndersenGraphPrinterPass(*PassRegistry::getPassRegistry());
  }

protected:
  virtual void process(const Data *Data, const Module &M) {
    printGraph(Data, &M);
  }
};

char AndersenGraphPrinter::ID = 0;

}

}

using namespace llvm;

INITIALIZE_PASS_BEGIN(AndersenGraphViewer, "view-andersen",
                      "View Andersen graph", false, true)
INITIALIZE_PASS_DEPENDENCY(AndersenPass)
INITIALIZE_PASS_END(AndersenGraphViewer, "view-andersen",
                    "View Andersen graph", false, true)

INITIALIZE_PASS_BEGIN(AndersenGraphPrinter, "dot-andersen",
                      "Print Andersen graph to 'dot' file", false, true)
INITIALIZE_PASS_DEPENDENCY(AndersenPass)
INITIALIZE_PASS_END(AndersenGraphPrinter, "dot-andersen",
                    "Print Andersen graph to 'dot' file", false, true)
