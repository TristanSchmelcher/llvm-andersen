//===- LazyAndersenIsNotNecessarilyEmptyIfMissingProperty.h ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the algorithm property that an algorithm's result is
// not necessarily empty if there is no cached value for it, regardless of the
// current phase.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENISNOTNECESSARILYEMPTYIFMISSINGPROPERTY_H
#define LAZYANDERSENISNOTNECESSARILYEMPTYIFMISSINGPROPERTY_H

#include "LazyAndersenPhase.h"

namespace llvm {
namespace lazyandersen {
  struct IsNotNecessarilyEmptyIfMissingProperty {
    template<Phase CurrentPhase>
    struct IsEmptyIfMissing {
      static const bool value = false;
    };
  };
}
}

#endif
