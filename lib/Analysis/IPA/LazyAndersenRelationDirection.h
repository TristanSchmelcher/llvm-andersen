//===- LazyAndersenRelationDirection.h - direction constants --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines direction constants for the LazyAndersen algorithm.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONDIRECTION_H
#define LAZYANDERSENRELATIONDIRECTION_H

namespace llvm {
namespace lazyandersen {
  enum RelationDirection {
    INCOMING,
    OUTGOING
  };

  template<RelationDirection Direction>
  struct DirectionTraits;

  template<>
  struct DirectionTraits<INCOMING> {
    static const RelationDirection OppositeDirection = OUTGOING;
  };

  template<>
  struct DirectionTraits<OUTGOING> {
    static const RelationDirection OppositeDirection = INCOMING;
  };
}
}

#endif
