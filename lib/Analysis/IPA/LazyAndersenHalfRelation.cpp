//===- LazyAndersenHalfRelation.cpp - relations classes -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the template type for one direction of a relation.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenHalfRelation.h"

using namespace llvm;
using namespace lazyandersen;

template<EdgeEndpointType Endpoint>
EdgeEndpointType HalfRelation<Endpoint>::getEdgeEndpointType() const {
  return Endpoint;
}

template<EdgeEndpointType Endpoint>
ValueInfo *HalfRelation<Endpoint>::getValueInfo() const {
  assert(getList());
  return ValueInfo::get(HalfRelationList<Endpoint>::from(
      HalfRelationBaseList::get(getList())));
}

template<EdgeEndpointType Endpoint>
HalfRelation<Endpoint>::~HalfRelation() {}

template class HalfRelation<SOURCE>;
template class HalfRelation<DESTINATION>;
