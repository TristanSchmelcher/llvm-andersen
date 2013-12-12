//===- LazyAndersenEnumerationResult.h - algorithm classes -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the return type for enumeration results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENENUMERATIONRESULT_H
#define LAZYANDERSENENUMERATIONRESULT_H

#include <cassert>

namespace llvm {
namespace lazyandersen {
  class AnalysisResult;
  class ValueInfo;

  class EnumerationResult {
  public:
    enum Type {
      NEXT_VALUE,
      RETRY,
      REWRITE,
      COMPLETE
    };

  private:
    Type type;
    union {
      ValueInfo *NextValue;
      AnalysisResult *AR;
      void *Unused;
    };

    explicit EnumerationResult(ValueInfo *NextValue)
      : type(NEXT_VALUE), NextValue(NextValue) {}

    EnumerationResult(Type type, AnalysisResult *AR)
      : type(type), AR(AR) {
      assert(type == RETRY || type == REWRITE);
    }

    EnumerationResult()
      : type(COMPLETE), Unused(0) {}

  public:
    static EnumerationResult makeNextValueResult(ValueInfo *NextValue) {
      return EnumerationResult(NextValue);
    }

    static EnumerationResult makeRetryResult(
        AnalysisResult *CancellationPoint) {
      return EnumerationResult(RETRY, CancellationPoint);
    }

    static EnumerationResult makeRewriteResult(
        AnalysisResult *Target) {
      return EnumerationResult(REWRITE, Target);
    }

    static EnumerationResult makeCompleteResult() {
      return EnumerationResult();
    }

    Type getResultType() const {
      return type;
    }

    ValueInfo *getNextValue() const {
      assert(type == NEXT_VALUE);
      return NextValue;
    }

    AnalysisResult *getRetryCancellationPoint() const {
      assert(type == RETRY);
      return AR;
    }

    AnalysisResult *getRewriteTarget() const {
      assert(type == REWRITE);
      return AR;
    }
  };
}
}

#endif
