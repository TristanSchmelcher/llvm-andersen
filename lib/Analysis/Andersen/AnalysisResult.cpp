//===- AnalysisResult.cpp - algorithm classes -----------------------------===//
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

#define DEBUG_TYPE "andersen"
#include "AnalysisResult.h"

#include "DebugInfo.h"
#include "EnumerationContext.h"
#include "EnumerationResult.h"
#include "SubsetWork.h"
#include "ValueInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <sstream>

namespace llvm {
namespace andersen_internal {

AnalysisResult::AnalysisResult() : EnumerationDepth(-1) {}

AnalysisResult::~AnalysisResult() {
  assert(!isEnumerating());
}

bool AnalysisResult::addValueInfo(ValueInfo *VI) {
  return Set.insert(VI);
}

bool AnalysisResult::prepareForSubset(AnalysisResult *Subset) {
  if (Subset == this) {
    // We could let it be added since it will trivially be elided, but
    // preventing it in the first place avoids an unnecessary entry in the set.
    return false;
  }
  // If we have added this subset to the work list before, don't add it again.
  return Subsets.insert(Subset).second;
}

EnumerationResult AnalysisResult::enumerate(int Depth, int LastTransformDepth,
    size_t &i) {
  assert(i <= Set.size());
  DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Enter " << this << '['
               << i << "]\n");
  // If not enumerating, EnumerationDepth is -1. If no last transform,
  // LastTransformDepth is -1.
  if (LastTransformDepth < EnumerationDepth) {
    DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
                 << '[' << i << "]: starting rewrite\n");
    // Infinite recursion of enumerate. Rewrite the chain of work to avoid it.
    return EnumerationResult::makeRewriteResult(this);
  }
  // Else check for cached result.
  if (i < Set.size()) {
    DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
                 << '[' << i << "]: cached " << Set[i] << '\n');
    return EnumerationResult::makeNextValueResult(Set[i++]);
  }
  // Else need to compute the next element.
  if (isEnumerating()) {
    DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
                 << '[' << i << "]: starting retry\n");
    // Application of a transform to a set containing itself. Defer.
    return EnumerationResult::makeRetryResult(this);
  }
  DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Run " << this << '['
               << i << "]\n");
  EnumerationContext Ctx(this, Depth, LastTransformDepth);
  AnalysisResult *RetryCancellationPoint = 0;
  while (Ctx.Pos != Work.end()) {
    EnumerationResult ER = Ctx.Pos->enumerate(&Ctx);
    switch (ER.getResultType()) {
    case EnumerationResult::NEXT_VALUE: {
      ValueInfo *VI = ER.getNextValue();
      if (addValueInfo(VI)) {
        DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
                     << '[' << i << "]: computed " << VI << '\n');
        ++i;
        return ER;
      }
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " In " << this
                   << '[' << i << "]: redundant " << VI << '\n');
      break;
    }

    case EnumerationResult::INLINE:
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
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
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " In " << this
                   << '[' << i << "]: retry at " << RetryCancellationPoint
                   << '\n');
      // Skip it. Will retry later if needed.
      ++Ctx.Pos;
      break;
    }

    case EnumerationResult::REWRITE: {
      // Move all other work to the rewrite target and replace this work list
      // with a reference to it.
      AnalysisResult *RewriteTarget = ER.getRewriteTarget();
      Ctx.Pos = Work.erase(Ctx.Pos);
      if (RewriteTarget != this) {
        assert(RewriteTarget->EnumerationDepth < EnumerationDepth);
        // Erase any work that shouldn't be moved.
        for (AnalysisResultWorkList::iterator j = Work.begin();
             j != Work.end(); ) {
          if (j->prepareForRewrite(RewriteTarget)) {
            ++j;
          } else {
            j = Work.erase(j);
          }
        }
        // Splice all remaining objects to the target.
        RewriteTarget->Work.splice(RewriteTarget->Work.end(), Work);
        // Not using appendSubset here because it could incorrectly elide the
        // new entry.
        Work.push_back(new SubsetWork(RewriteTarget));
        DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
                     << '[' << i << "]: rewriting " << RewriteTarget << '\n');
        return ER;
      }
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " In " << this
                   << '[' << i << "]: finished rewrite\n");
      break;
    }

    case EnumerationResult::COMPLETE:
      Ctx.Pos = Work.erase(Ctx.Pos);
      break;

    default:
      llvm_unreachable("Not a recognized EnumerationResult");
      break;
    }
  }
  // Nothing new added to Set. Either we're done or we need a retry.
  if (!RetryCancellationPoint) {
    assert(Work.empty());
    DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
                 << '[' << i << "]: end of set\n");
    return EnumerationResult::makeCompleteResult();
  } else {
    assert(!Work.empty());
    if (RetryCancellationPoint == this) {
      // This AR has self-contained reference cycles but this iteration found
      // no new items. Therefore it's done. Cancel the retry state.
      Work.clear();
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
                   << '[' << i << "]: finished retry\n");
      return EnumerationResult::makeCompleteResult();
    } else {
      // This AR has non-self-contained reference cycles. Must retry
      // enumeration later.
      assert(RetryCancellationPoint->EnumerationDepth < EnumerationDepth);
      DEBUG(dbgs() << Depth << ':' << LastTransformDepth << " Leave " << this
                   << '[' << i << "]: retrying " << RetryCancellationPoint
                   << '\n');
      return EnumerationResult::makeRetryResult(RetryCancellationPoint);
    }
  }
}

void AnalysisResult::writeEquation(const DebugInfo &DI, raw_ostream &OS) const {
  DI.printAnalysisResultName(this, OS);
  OS << " = ";
  bool first = true;
  if (!Set.empty()) {
    OS << '{';
    ValueInfoSetVector::const_iterator i = Set.begin(), End = Set.end();
    do {
      if (!first) {
        OS << ", ";
      }
      first = false;
      DI.printValueInfoName(*i, OS);
    } while (++i != End);
    OS << '}';
  }
  for (AnalysisResultWorkList::const_iterator i = Work.begin(),
                                              End = Work.end();
       i != End; ++i) {
    if (!first) {
      OS << " U ";
    }
    first = false;
    i->writeFormula(DI, OS);
  }
  if (first) {
    // Empty.
    OS << "{}";
  }
  OS << '\n';
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
  for (AnalysisResultWorkList::const_iterator i = Work.begin(),
                                              End = Work.end();
       i != End; ++i, ++Pos) {
    const AnalysisResultWork *Ent = &*i;
    std::ostringstream OSS;
    OSS << Pos;
    Result.push_back(GraphEdge(Ent, OSS.str()));
  }
  return Result;
}

void AnalysisResult::printNodeLabel(const DebugInfo &DI, raw_ostream &OS) const {
  DI.printAnalysisResultName(this, OS);
}

bool AnalysisResult::isNodeHidden() const {
  return false;
}

}
}
