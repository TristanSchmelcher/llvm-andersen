//===- EnumerationResult.h - algorithm classes ----------------------------===//
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

#ifndef ENUMERATIONRESULT_H
#define ENUMERATIONRESULT_H

#include <cassert>

namespace llvm {
namespace andersen_internal {

class AnalysisResult;
class Enumerator;
class ValueInfo;

class EnumerationResult {
public:
  enum Type {
    NEXT_VALUE,
    INLINE,
    RETRY,
    REWRITE,
    COMPLETE
  };

private:
  Type type;
  union {
    ValueInfo *NextValue;
    Enumerator *InlineEnumerator;
    AnalysisResult *AR;
    void *Unused;
  };

  explicit EnumerationResult(ValueInfo *NextValue)
    : type(NEXT_VALUE), NextValue(NextValue) {}

  explicit EnumerationResult(Enumerator *InlineEnumerator)
    : type(INLINE), InlineEnumerator(InlineEnumerator) {}

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

  static EnumerationResult makeInlineResult(Enumerator *InlineEnumerator) {
    return EnumerationResult(InlineEnumerator);
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

  Enumerator *getInlineEnumerator() const {
    assert(type == INLINE);
    return InlineEnumerator;
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
