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
#include "LazyAndersenHalfRelation-inl.h"
#include "LazyAndersenHalfRelationBase-inl.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Value.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

ValueInfo::Map::value_type *IncomingHalfRelationIteratorAdapter::
    AdapterFunctor::operator()(const HalfRelationBase &HR) const {
  ValueInfo *VI = HR.as<INCOMING>()->getOppositeDirection()
      ->getValueInfo();
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

namespace {
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

  inline std::string getPrintedValue(const Value *V) {
    std::ostringstream OSS;
    {
      raw_os_ostream OS(OSS);
      V->print(OS);
    }
    return OSS.str();
  }
}

DOTGraphTraits<ValueInfo::Map>::DOTGraphTraits(bool simple)
  : DefaultDOTGraphTraits(simple) {}

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
namespace llvm {
namespace lazyandersen {
void viewLazyAndersenGraph(LazyAndersenData *Data) {
  ViewGraph(Data->ValueInfos, "LazyAndersen", false,
      "LazyAndersen analysis results");
}
}
}
#endif
