//===- llvm/Analysis/Enumerator.cpp - internal enumerator class -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the internal Enumerator class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "andersen"
#include "Enumerator.h"

#include "AnalysisResult.h"
#include "DebugInfo.h"
#include "EnumerationContext.h"
#include "EnumerationResult.h"
#include "GraphNode.h"
#include "RecursiveEnumerate.h"
#include "llvm/Analysis/AndersenEnumerator.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <sstream>

namespace llvm {
namespace andersen_internal {

EnumerationResult Enumerator::enumerate(int Depth, int LastTransformDepth) {
  assert(i <= AR->Set.size());
  DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Enter " << AR << '['
               << i << "]\n");
  // If not enumerating, AR->EnumerationDepth is -1. If no last transform,
  // LastTransformDepth is -1.
  if (LastTransformDepth < AR->EnumerationDepth) {
    DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR << '['
                 << i << "]: starting rewrite\n");
    // Infinite recursion of enumerate. Rewrite the chain of work to avoid it.
    return EnumerationResult::makeRewriteResult(AR);
  }
  // Else check for cached result.
  if (i < AR->Set.size()) {
    DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR << '['
                 << i << "]: cached " << AR->Set[i] << '\n');
    return EnumerationResult::makeNextValueResult(AR->Set[i++]);
  }
  // Else need to compute the next element.
  if (AR->isEnumerating()) {
    DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR << '['
                 << i << "]: starting retry\n");
    // Application of a transform to a set containing itself. Defer.
    return EnumerationResult::makeRetryResult(AR);
  }
  DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Run " << AR << '['
               << i << "]\n");
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
        DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR
                     << '[' << i << "]: computed " << VI << '\n');
        ++i;
        return ER;
      }
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " In " << AR << '['
                   << i << "]: redundant " << VI << '\n');
      break;
    }

    case EnumerationResult::INLINE:
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR
                   << '[' << i << "]: inlining "
                   << ER.getInlineEnumerator()->getAnalysisResult() << '['
                   << ER.getInlineEnumerator()->getPosition() << ":]\n");
      return ER;

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
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " In " << AR << '['
                   << i << "]: retry at " << RetryCancellationPoint << '\n');
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
        // Erase any work that shouldn't be moved.
        for (AnalysisResultWorkList::iterator j = AR->Work.begin();
             j != AR->Work.end(); ) {
          if (j->prepareForRewrite(RewriteTarget)) {
            ++j;
          } else {
            j = AR->Work.erase(j);
          }
        }
        // Splice all remaining objects to the target.
        RewriteTarget->Work.splice(RewriteTarget->Work.end(), AR->Work);
        // Not using appendSubset here because it could incorrectly elide the
        // new entry.
        AR->Work.push_back(new RecursiveEnumerate(RewriteTarget));
        DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR
                     << '[' << i << "]: rewriting " << RewriteTarget << '\n');
        return ER;
      }
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " In " << AR << '['
                   << i << "]: finished rewrite\n");
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
    DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR << '['
                 << i << "]: end of set\n");
    return EnumerationResult::makeCompleteResult();
  } else {
    assert(!AR->Work.empty());
    if (RetryCancellationPoint == AR) {
      // This AR has self-contained reference cycles but this iteration found
      // no new items. Therefore it's done. Cancel the retry state.
      AR->Work.clear();
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR
                   << '[' << i << "]: finished retry\n");
      return EnumerationResult::makeCompleteResult();
    } else {
      // This AR has non-self-contained reference cycles. Must retry
      // enumeration later.
      assert(RetryCancellationPoint->EnumerationDepth < AR->EnumerationDepth);
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << AR
                   << '[' << i << "]: retrying " << RetryCancellationPoint
                   << '\n');
      return EnumerationResult::makeRetryResult(RetryCancellationPoint);
    }
  }
}

GraphEdge Enumerator::toGraphEdge() const {
  std::ostringstream OSS;
  OSS << "Index " << i;
  return GraphEdge(AR, OSS.str());
}

void Enumerator::writeFormula(const DebugInfo &DI, raw_ostream &OS) const {
  DI.printAnalysisResultName(AR, OS);
  OS << '[' << i << ":]";
}

}
}
