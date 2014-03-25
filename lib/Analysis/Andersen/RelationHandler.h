//===- RelationHandler.h --------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a class for translating the relations produced by
// instruction analysis into graph traversal steps.
//
//===----------------------------------------------------------------------===//

#ifndef RELATIONHANDLER_H
#define RELATIONHANDLER_H

namespace llvm {

class Instruction;

}

namespace llvm {
namespace andersen_internal {

class Data;
class ValueInfo;

enum BinaryRelation {
  ARGUMENT_FROM_CALLER,
  DEPENDS_ON,
  LOADED_FROM,
  RETURNED_TO_CALLER,
  STORED_TO,
  READS_FROM,
  WRITES_TO
};

enum TrinaryRelation {
  ARGUMENT_TO_CALLEE,
  RETURNED_FROM_CALLEE,
  CALLS
};

class RelationHandler {
public:
  Data *const D;

  explicit RelationHandler(Data *D) : D(D) {}

  // Generates AnalysisResult entries to handle the relation RT between Src and
  // Dst.
  template<BinaryRelation RT>
  static void handleRelation(ValueInfo *Src, ValueInfo *Dst);

  // Generates AnalysisResult entries to handle the relation RT between Src and
  // Dst for a given callsite.
  template<TrinaryRelation RT>
  void handleRelation(ValueInfo *Src, ValueInfo *Dst,
      const Instruction *CallSite);
};

}
}

#endif
