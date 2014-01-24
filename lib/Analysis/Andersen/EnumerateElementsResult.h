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
    RETRY_START,
    RETRY_CONTINUE,
    ELEMENT
  };

private:
  Type type;
  union {
    void *Unused;
    EnumerationState *RetrySource;
    ValueInfo *Element;
  };

  EnumerateElementsResult()
    : type(COMPLETE), Unused(0) {}

  EnumerateElementsResult(Type type, EnumerationState *RetrySource)
    : type(type), RetrySource(RetrySource) {
    assert(type == RETRY_START || type == RETRY_CONTINUE);
  }

  explicit EnumerateElementsResult(ValueInfo *Element)
    : type(ELEMENT), Element(Element) {}

public:
  static EnumerateElementsResult makeCompleteResult() {
    return EnumerateElementsResult();
  }

  static EnumerateElementsResult makeRetryStartResult(
      EnumerationState *RetrySource) {
    return EnumerateElementsResult(RETRY_START, RetrySource);
  }

  static EnumerateElementsResult makeRetryContinueResult(
      EnumerationState *RetrySource) {
    return EnumerateElementsResult(RETRY_CONTINUE, RetrySource);
  }

  static EnumerateElementsResult makeElementResult(ValueInfo *Element) {
    return EnumerateElementsResult(Element);
  }

  Type getResultType() const {
    return type;
  }

  EnumerationState *getRetrySource() const {
    assert(type == RETRY_START || type == RETRY_CONTINUE);
    return RetrySource;
  }

  ValueInfo *getElement() const {
    assert(type == ELEMENT);
    return Element;
  }
};

}
}

#endif
