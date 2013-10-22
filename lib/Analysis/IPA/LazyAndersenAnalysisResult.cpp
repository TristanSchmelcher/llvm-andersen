//===- LazyAndersenAnalysisResult.cpp - algorithm classes -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for analysis results.
//
//===----------------------------------------------------------------------===//

#include "LazyAndersenAnalysisResult.h"

#include "LazyAndersenAnalysisResultCacheEntry.h"
#include "LazyAndersenAnalysisResultEntryBase.h"
#include "LazyAndersenAnalysisResultPendingWorkEntry.h"
#include "LazyAndersenRecursiveEnumerate.h"
#include "LazyAndersenValueInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"

#include <sstream>

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisResult::AnalysisResult() : EnumerationDepth(-1) {}

AnalysisResult::~AnalysisResult() {
  assert(!isEnumerating());
}

GraphEdgeDeque AnalysisResult::getOutgoingEdges() const {
  GraphEdgeDeque Result;
  size_t Pos = 0;
  for (ValueInfoSetVector::const_iterator i = Set.begin(), End = Set.end();
       i != End; ++i, ++Pos) {
    std::ostringstream OSS;
    OSS << Pos;
    Result.push_back(GraphEdge(*i, OSS.str()));
  }
  for (AnalysisResultEntryBaseList::const_iterator i = Work.begin(),
                                                   End = Work.end();
       i != End; ++i, ++Pos) {
    const AnalysisResultEntryBase *Ent = &*i;
    std::ostringstream OSS;
    OSS << Pos;
    Result.push_back(GraphEdge(Ent->getGraphNode(), OSS.str()));
  }
  return Result;
}

std::string AnalysisResult::getNodeLabel() const {
  return std::string();
}

bool AnalysisResult::isNodeHidden() const {
  return false;
}

inline
AnalysisResult::Enumerator::ScopedSetEnumerating::ScopedSetEnumerating(
    AnalysisResult *AR, int Depth) : AR(AR) {
  assert(!AR->isEnumerating());
  assert(Depth >= 0);
  AR->EnumerationDepth = Depth;
}

inline
AnalysisResult::Enumerator::ScopedSetEnumerating::~ScopedSetEnumerating() {
  assert(AR->isEnumerating());
  AR->EnumerationDepth = -1;
}

inline
AnalysisResult::Enumerator::Context::Context(AnalysisResult *AR, int Depth)
  : ScopedSetEnumerating(AR, Depth),
    NextDepth(Depth + 1),
    Pos(AR->Work.begin()) {}

AnalysisResult::EnumerationResult AnalysisResult::Enumerator::Context::pushWork(
    AnalysisResult *Child) {
  RecursiveEnumerate *RE = new RecursiveEnumerate(Child);
  Pos = AR->Work.insert(Pos, RE);
  return RE->enumerate(this);
}

AnalysisResult::EnumerationResult AnalysisResult::Enumerator::enumerate(
    int Depth) {
  assert(i <= AR->Set.size());
  if (i < AR->Set.size()) {
    return EnumerationResult::makeNextValueResult(AR->Set[i++]);
  }
  // Else need to compute the next element.
  if (AR->isEnumerating()) {
    // We have entered a loop. Defer.
    return EnumerationResult::makeRetryResult(AR);
  }
  Context Ctx(AR, Depth);
  AnalysisResult *RetryCancellationPoint = 0;
  while (Ctx.Pos != AR->Work.end()) {
    switch (Ctx.Pos->getEntryType()) {
    case AnalysisResultEntryBase::VALUE_INFO_ENTRY: {
      // TODO: Since this is so trivial, we should eliminate VALUE_INFO_ENTRY
      // entirely.
      ValueInfo *VI = cast<AnalysisResultValueInfoEntry>(&*Ctx.Pos)
          ->getCachedValue();
      Ctx.Pos = AR->Work.erase(Ctx.Pos);
      if (AR->Set.insert(VI)) {
        ++i;
        return EnumerationResult::makeNextValueResult(VI);
      }
      break;
    }

    case AnalysisResultEntryBase::PENDING_WORK_ENTRY: {
      EnumerationResult ER =
          cast<AnalysisResultPendingWorkEntry>(&*Ctx.Pos)->enumerate(&Ctx);
      switch (ER.getResultType()) {
      case EnumerationResult::NEXT_VALUE: {
        ValueInfo *VI = ER.getNextValue();
        if (AR->Set.insert(VI)) {
          ++i;
          return EnumerationResult::makeNextValueResult(VI);
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

      case EnumerationResult::COMPLETE:
        Ctx.Pos = AR->Work.erase(Ctx.Pos);
        break;

      default:
        llvm_unreachable("Not a recognized EnumerationResult");
        break;
      }
      break;
    }

    default:
      llvm_unreachable("Not a recognized AnalysisResultEntry");
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

GraphEdge AnalysisResult::Enumerator::toGraphEdge() const {
  std::ostringstream OSS;
  OSS << "Index " << i;
  return GraphEdge(AR, OSS.str());
}
