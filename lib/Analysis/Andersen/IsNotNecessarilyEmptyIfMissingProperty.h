//===- IsNotNecessarilyEmptyIfMissingProperty.h ---------------------------===//
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

#ifndef ISNOTNECESSARILYEMPTYIFMISSINGPROPERTY_H
#define ISNOTNECESSARILYEMPTYIFMISSINGPROPERTY_H

#include "Phase.h"

namespace llvm {
namespace andersen_internal {
  struct IsNotNecessarilyEmptyIfMissingProperty {
    template<Phase CurrentPhase>
    struct IsEmptyIfMissing {
      static const bool value = false;
    };
  };
}
}

#endif
