//===- IsEmptyIfMissingInEnumerationPhaseProperty.h -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the algorithm property that an algorithm's result is
// implicitly empty if there is no cached value for it in the enumeration phase.
//
//===----------------------------------------------------------------------===//

#ifndef ISEMPTYIFMISSINGINENUMERATIONPHASEPROPERTY_H
#define ISEMPTYIFMISSINGINENUMERATIONPHASEPROPERTY_H

#include "Phase.h"

namespace llvm {
namespace andersen_internal {

struct IsEmptyIfMissingInEnumerationPhaseProperty {
  template<Phase CurrentPhase>
  struct IsEmptyIfMissing;
};

template<>
struct IsEmptyIfMissingInEnumerationPhaseProperty::IsEmptyIfMissing<
    INSTRUCTION_ANALYSIS_PHASE> {
  static const bool value = false;
};

template<>
struct IsEmptyIfMissingInEnumerationPhaseProperty::IsEmptyIfMissing<
    ENUMERATION_PHASE> {
  static const bool value = true;
};

}
}

#endif
