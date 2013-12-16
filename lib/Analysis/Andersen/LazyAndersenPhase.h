//===- LazyAndersenPhase.h - algorithm classes ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares an enum for the phases of the Andersen analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENPHASE_H
#define LAZYANDERSENPHASE_H

namespace llvm {
namespace andersen_internal {
  enum Phase {
    // In the instruction analysis phase, we do a O(n) scan of the code and
    // build work lists for algorithms' AnalysisResults.
    INSTRUCTION_ANALYSIS_PHASE,
    // In the enumeration phase, we execute work lists lazily in response to
    // points-to set queries.
    ENUMERATION_PHASE
  };
}
}

#endif
