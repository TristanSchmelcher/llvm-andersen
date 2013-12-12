//===- LazyAndersenEnumerator.cpp - algorithm classes ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for enumerating an analysis result's contents.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenEnumerator.h"

#include "LazyAndersenAnalysisResult.h"
#include "LazyAndersenEnumerationContext.h"
#include "LazyAndersenRecursiveEnumerate.h"
#include "llvm/Support/ErrorHandling.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

Enumerator::Enumerator(AnalysisResult *AR, size_t i) : AR(AR), i(i) {
  assert(AR);
  assert(i <= AR->Set.size());
}

EnumerationResult Enumerator::enumerate(int Depth, int LastTransformDepth) {
  assert(i <= AR->Set.size());
  // If not enumerating, AR->EnumerationDepth is -1. If no last transform,
  // LastTransformDepth is -1.
  if (LastTransformDepth < AR->EnumerationDepth) {
    // Infinite recursion of enumerate. Rewrite the chain of work to avoid it.
    return EnumerationResult::makeRewriteResult(AR);
  }
  // Else check for cached result.
  if (i < AR->Set.size()) {
    return EnumerationResult::makeNextValueResult(AR->Set[i++]);
  }
  // Else need to compute the next element.
  if (AR->isEnumerating()) {
    // Application of a transform to a set containing itself. Defer.
    return EnumerationResult::makeRetryResult(AR);
  }
  EnumerationContext Ctx(AR, Depth, LastTransformDepth);
  AnalysisResult *RetryCancellationPoint = 0;
  // TODO: When there's just a single RecursiveEnumerate work item, we can
  // "inline" it into the caller.
  while (Ctx.Pos != AR->Work.end()) {
    EnumerationResult ER = Ctx.Pos->enumerate(&Ctx);
    switch (ER.getResultType()) {
    case EnumerationResult::NEXT_VALUE: {
      ValueInfo *VI = ER.getNextValue();
      if (AR->addValueInfo(VI)) {
        ++i;
        return ER;
      }
      break;
    }

    case EnumerationResult::RETRY: {
      AnalysisResult *NewRetryCancellationPoint =
          ER.getRetryCancellationPoint();
      assert(NewRetryCancellationPoint->isEnumerating());
      // Overall retry cancellation point is the one with the least depth.
      if (!RetryCancellationPoint) {
        RetryCancellationPoint = NewRetryCancellationPoint;
      } else {
        assert(RetryCancellationPoint->isEnumerating());
        if (NewRetryCancellationPoint->EnumerationDepth <
            RetryCancellationPoint->EnumerationDepth) {
          RetryCancellationPoint = NewRetryCancellationPoint;
        }
      }
      // Skip it. Will retry later if needed.
      ++Ctx.Pos;
      break;
    }

    case EnumerationResult::REWRITE: {
      // Move all other work to the rewrite target and replace this work list
      // with a reference to it.
      AnalysisResult *RewriteTarget = ER.getRewriteTarget();
      Ctx.Pos = AR->Work.erase(Ctx.Pos);
      if (RewriteTarget != AR) {
        assert(RewriteTarget->EnumerationDepth < AR->EnumerationDepth);
        RewriteTarget->Work.splice(RewriteTarget->Work.end(),
                                   AR->Work);
        AR->addWork(new RecursiveEnumerate(RewriteTarget));
        return ER;
      }
      break;
    }

    case EnumerationResult::COMPLETE:
      Ctx.Pos = AR->Work.erase(Ctx.Pos);
      break;

    default:
      llvm_unreachable("Not a recognized EnumerationResult");
      break;
    }
  }
  // Nothing new added to Set. Either we're done or we need a retry.
  if (!RetryCancellationPoint) {
    assert(AR->Work.empty());
    return EnumerationResult::makeCompleteResult();
  } else {
    assert(!AR->Work.empty());
    if (RetryCancellationPoint == AR) {
      // This AR has self-contained reference cycles but this iteration found
      // no new items. Therefore it's done. Cancel the retry state.
      AR->Work.clear();
      return EnumerationResult::makeCompleteResult();
    } else {
      // This AR has non-self-contained reference cycles. Must retry
      // enumeration later.
      assert(RetryCancellationPoint->EnumerationDepth < AR->EnumerationDepth);
      return EnumerationResult::makeRetryResult(RetryCancellationPoint);
    }
  }
}

GraphEdge Enumerator::toGraphEdge() const {
  std::ostringstream OSS;
  OSS << "Index " << i;
  return GraphEdge(AR, OSS.str());
}
