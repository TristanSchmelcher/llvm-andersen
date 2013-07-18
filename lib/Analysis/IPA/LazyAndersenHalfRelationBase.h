//===- LazyAndersenHalfRelationBase.h - relations classes -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the base type for one direction of a relation.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENHALFRELATIONBASE_H
#define LAZYANDERSENHALFRELATIONBASE_H

#include "LazyAndersenEdgeEndpointType.h"
#include "LazyAndersenIntrusiveListNode.h"
#include "LazyAndersenIntrusiveListTraits.h"

namespace llvm {
namespace lazyandersen {
  template<EdgeEndpointType Endpoint> class HalfRelation;
  class HalfRelationBaseList;

  class HalfRelationBase : public HasEdgeEndpointType,
      private IntrusiveListNode<HalfRelationBase> {
    friend struct ilist_ghostly_sentinel_traits<HalfRelationBase>;
    friend struct ilist_nextprev_traits<HalfRelationBase>;
    friend struct ilist_node_traits<HalfRelationBase>;
    friend struct IntrusiveListTraits<HalfRelationBase>;
    template<EdgeEndpointType> friend class HalfRelation;

  protected:
    using IntrusiveListNode<HalfRelationBase>::getList;

  private:
    explicit HalfRelationBase(HalfRelationBaseList *InitialList);
    // virtual so that deleteNode in ilist_node_traits will delete the whole
    // Relation object properly.
    virtual ~HalfRelationBase();
  };
}
}

namespace llvm {
  template<>
  struct ilist_traits<lazyandersen::HalfRelationBase> :
      public lazyandersen::IntrusiveListTraits<
          lazyandersen::HalfRelationBase> {};
}

#endif
