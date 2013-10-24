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
      COMPLETE
    };

  private:
    Type type;
    union {
      ValueInfo *NextValue;
      AnalysisResult *CancellationPoint;
      void *Unused;
    };

    explicit EnumerationResult(ValueInfo *NextValue)
      : type(NEXT_VALUE), NextValue(NextValue) {}

    explicit EnumerationResult(AnalysisResult *CancellationPoint)
      : type(RETRY), CancellationPoint(CancellationPoint) {}

    EnumerationResult()
      : type(COMPLETE), Unused(0) {}

  public:
    static EnumerationResult makeNextValueResult(ValueInfo *NextValue) {
      return EnumerationResult(NextValue);
    }

    static EnumerationResult makeRetryResult(
        AnalysisResult *CancellationPoint) {
      return EnumerationResult(CancellationPoint);
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
      return CancellationPoint;
    }
  };
}
}

#endif
