//===- LazyAndersenEdgeEndpointType.cpp - graph edge constants ------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines edge endpoint constants and traits for the LazyAndersen
// algorithm.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenEdgeEndpointType.h"

namespace llvm {
namespace lazyandersen {

const char EdgeEndpointTraitsBase<SOURCE>::EdgeEndpointTypeShortName[] =
    "SRC";

const char EdgeEndpointTraitsBase<DESTINATION>::EdgeEndpointTypeShortName[] =
    "DST";

}
}
