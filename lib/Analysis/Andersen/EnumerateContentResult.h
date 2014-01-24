//===- EnumerateContentResult.h - algorithm classes -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the return type for AnalysisResult::enumerateContent().
//
//===----------------------------------------------------------------------===//

#ifndef ENUMERATECONTENTRESULT_H
#define ENUMERATECONTENTRESULT_H

#include <cassert>

namespace llvm {
namespace andersen_internal {

class AnalysisResult;
class EnumerationState;
class ValueInfo;

class EnumerateContentResult {
public:
  enum Type {
    COMPLETE,
    RETRY,
    VALUE_INFO_CONTENT,
    SUBSET_CONTENT
  };

private:
  Type type;
  union {
    void *Unused;
    EnumerationState *RetryCancellationPoint;
    ValueInfo *ValueInfoContent;
    AnalysisResult *SubsetContent;
  };

  EnumerateContentResult()
    : type(COMPLETE), Unused(0) {}

  explicit EnumerateContentResult(EnumerationState *RetryCancellationPoint)
    : type(RETRY), RetryCancellationPoint(RetryCancellationPoint) {}

  explicit EnumerateContentResult(ValueInfo *ValueInfoContent)
    : type(VALUE_INFO_CONTENT), ValueInfoContent(ValueInfoContent) {}

  explicit EnumerateContentResult(AnalysisResult *SubsetContent)
    : type(SUBSET_CONTENT), SubsetContent(SubsetContent) {}

public:
  static EnumerateContentResult makeCompleteResult() {
    return EnumerateContentResult();
  }

  static EnumerateContentResult makeRetryResult(
      EnumerationState *RetryCancellationPoint) {
    return EnumerateContentResult(RetryCancellationPoint);
  }

  static EnumerateContentResult makeValueInfoContentResult(
      ValueInfo *ValueInfoContent) {
    return EnumerateContentResult(ValueInfoContent);
  }

  static EnumerateContentResult makeSubsetContentResult(
      AnalysisResult *SubsetContent) {
    return EnumerateContentResult(SubsetContent);
  }

  Type getResultType() const {
    return type;
  }

  EnumerationState *getRetryCancellationPoint() const {
    assert(type == RETRY);
    return RetryCancellationPoint;
  }

  ValueInfo *getValueInfoContent() const {
    assert(type == VALUE_INFO_CONTENT);
    return ValueInfoContent;
  }

  AnalysisResult *getSubsetContent() const {
    assert(type == SUBSET_CONTENT);
    return SubsetContent;
  }
};

}
}

#endif
