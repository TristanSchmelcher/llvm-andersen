//===- InstructionAnalyzer.h - instruction analysis -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the type for instruction analysis.
//
//===----------------------------------------------------------------------===//

#ifndef INSTRUCTIONANALYZER_H
#define INSTRUCTIONANALYZER_H

namespace llvm {
  class Module;
  class ModulePass;
}

namespace llvm {
namespace andersen_internal {
  class Data;

  class InstructionAnalyzer {
    class Visitor;

  public:
    static Data *run(ModulePass *MP, Module &M);
  };
}
}

#endif
