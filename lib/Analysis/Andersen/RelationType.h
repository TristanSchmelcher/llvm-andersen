//===- RelationType.h - relation classes ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares an enum for relation types.
//
//===----------------------------------------------------------------------===//

#ifndef RELATIONTYPE_H
#define RELATIONTYPE_H

namespace llvm {
namespace andersen_internal {
  enum RelationType {
    ARGUMENT_FROM_CALLER,
    ARGUMENT_TO_CALLEE,
    DEPENDS_ON,
    LOADED_FROM,
    RETURNED_FROM_CALLEE,
    RETURNED_TO_CALLER,
    STORED_TO
  };
}
}

#endif
