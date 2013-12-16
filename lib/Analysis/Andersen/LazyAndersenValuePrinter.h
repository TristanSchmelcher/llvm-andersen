//===- LazyAndersenValuePrinter.h - value printing helper routine ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a helper method for printing Values.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENVALUEPRINTER_H
#define LAZYANDERSENVALUEPRINTER_H

#include <limits>
#include <string>

namespace llvm {
  class Value;
}

namespace llvm {
namespace andersen_internal {
  // Returns a string containing the first line of the printed form of V plus
  // its name (if any). If the first line exceeds MaxPrintedSize characters, it
  // is truncated and " ..." is added. The name is never truncated.
  std::string prettyPrintValue(const Value *V,
      size_t MaxPrintedSize = std::numeric_limits<size_t>::max());
}
}

#endif