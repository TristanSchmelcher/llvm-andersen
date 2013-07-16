//===- LazyAndersenGraphTraits.cpp - graph traits -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines graph traits specializations for graph printing.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenGraphTraits.h"

#include "LazyAndersenData.h"
#include "LazyAndersenRelation.h"
#include "LazyAndersenValuePrinter.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_os_ostream.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

ValueInfo::Map::value_type *IncomingHalfRelationIteratorAdapter::
    AdapterFunctor::operator()(const HalfRelationBase &HR) const {
  ValueInfo *VI = Relation::getOppositeDirection(HalfRelation<INCOMING>::from(
      &HR))->getValueInfo();
  return &*VI->getMap()->find(VI->getValue());
}

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

DOTGraphTraits<ValueInfo::Map>::DOTGraphTraits(bool simple)
  : DefaultDOTGraphTraits(simple) {}

std::string DOTGraphTraits<ValueInfo::Map>::getNodeLabel(
    GraphTraits<ValueInfo::Map>::NodeType *Node,
    const ValueInfo::Map &Map) {
  static const size_t MaxPrintedSize = 16;
  const Value *V = Node->first;
  return prettyPrintValue(V, MaxPrintedSize);
}

std::string DOTGraphTraits<ValueInfo::Map>::getEdgeSourceLabel(
    GraphTraits<ValueInfo::Map>::NodeType *Node,
    const GraphTraits<ValueInfo::Map>::ChildIteratorType &i) {
  return Relation::get(HalfRelation<INCOMING>::from(i.wrappedIterator()))
      ->getRelationName();
}

bool DOTGraphTraits<ValueInfo::Map>::isNodeHidden(
    GraphTraits<ValueInfo::Map>::NodeType *Node) {
  return !Node->second.getPtr() || Node->first != Node->second->getValue();
}

namespace llvm {
namespace lazyandersen {
void viewLazyAndersenGraph(LazyAndersenData *Data, const Module *M) {
  std::ostringstream OSS;
  OSS << "LazyAndersen analysis results";
  if (M) {
    OSS << " for module " << M->getModuleIdentifier();
  }
  ViewGraph(Data->ValueInfos, "LazyAndersen", false, OSS.str());
}
}
}
