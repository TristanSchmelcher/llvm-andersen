//===- EnumerateElementsResult.h - algorithm classes ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the return type for AnalysisResult::enumerateElements().
//
//===----------------------------------------------------------------------===//

#ifndef ENUMERATEELEMENTSRESULT_H
#define ENUMERATEELEMENTSRESULT_H

#include <cassert>

namespace llvm {
namespace andersen_internal {

class EnumerationState;
class ValueInfo;

class EnumerateElementsResult {
public:
  enum Type {
    COMPLETE,
    RETRY,
    ELEMENT
  };

private:
  Type type;
  union {
    void *Unused;
    EnumerationState *RetryCancellationPoint;
    ValueInfo *Element;
  };

  EnumerateElementsResult()
    : type(COMPLETE), Unused(0) {}

  explicit EnumerateElementsResult(EnumerationState *RetryCancellationPoint)
    : type(RETRY), RetryCancellationPoint(RetryCancellationPoint) {}

  explicit EnumerateElementsResult(ValueInfo *Element)
    : type(ELEMENT), Element(Element) {}

public:
  static EnumerateElementsResult makeCompleteResult() {
    return EnumerateElementsResult();
  }

  static EnumerateElementsResult makeRetryResult(
      EnumerationState *RetryCancellationPoint) {
    return EnumerateElementsResult(RetryCancellationPoint);
  }

  static EnumerateElementsResult makeElementResult(ValueInfo *Element) {
    return EnumerateElementsResult(Element);
  }

  Type getResultType() const {
    return type;
  }

  EnumerationState *getRetryCancellationPoint() const {
    assert(type == RETRY);
    return RetryCancellationPoint;
  }

  ValueInfo *getElement() const {
    assert(type == ELEMENT);
    return Element;
  }
};

}
}

#endif
