//===- LazyAndersenHalfRelationList.cpp - relations classes ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the template type for a list of relations for a particular
// direction.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenHalfRelationList.h"

using namespace llvm;
using namespace lazyandersen;

template<EdgeEndpointType Endpoint>
EdgeEndpointType HalfRelationList<Endpoint>::getEdgeEndpointType() const {
  return Endpoint;
}

template<EdgeEndpointType Endpoint>
HalfRelationList<Endpoint>::HalfRelationList() {}

template class HalfRelationList<SOURCE>;
template class HalfRelationList<DESTINATION>;
