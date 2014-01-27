//===- EnumerationState.cpp -----------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#include "EnumerationState.h"

#include "AnalysisResult.h"
#include "DebugInfo.h"
#include "EnumerateContentResult.h"
#include "EnumerateElementsResult.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {
namespace andersen_internal {

class EnumerationState::ScopedEnumerateContext {
  EnumerationState *const ES;

public:
  ScopedEnumerateContext(EnumerationState *ES, int Depth)
    : ES(ES) {
    assert(!ES->isEnumerating());
    assert(Depth >= 0);
    ES->EnumerationDepth = Depth;
  }

  ~ScopedEnumerateContext() {
    assert(ES->isEnumerating());
    ES->EnumerationDepth = -1;
    ++ES->ExitCount;
  }
};

inline EnumerationState::ContentEnumerator::ContentEnumerator(
    AnalysisResult *AR)
  : AR(AR), j(0) {}

EnumerationState::EnumerationState(AnalysisResult *AR)
  : EnumerationDepth(-1), ExitCount(0) {
  addSubset(AR);
}

EnumerateElementsResult EnumerationState::enumerateElements(size_t i,
    int Depth) {
  assert(i <= Elements.size());
  if (i < Elements.size()) {
    return EnumerateElementsResult::makeElementResult(Elements[i]);
  }
  if (!isDone()) {
    if (isEnumerating()) {
      return EnumerateElementsResult::makeRetryResult(this);
    }
    ScopedEnumerateContext Enumerating(this, Depth++);
    EnumerationState *RetryCancellationPoint = 0;
    AnalysisResult *FirstRetrySource = 0;
    do {
      ContentEnumerator &CE(ContentEnumerators.front());
      if (CE.AR == FirstRetrySource) {
        // We've looped around and not found any new elements.
        assert(RetryCancellationPoint);
        assert(RetryCancellationPoint->isEnumerating());
        if (this == RetryCancellationPoint) {
          // We have self-contained reference cycles and this iteration found
          // no new items. Therefore we're done. Cancel the retry state.
          ContentEnumerators.clear();
          break;
        } else {
          // We have non-self-contained reference cycles. Must retry
          // enumeration later.
          return EnumerateElementsResult::makeRetryResult(
              RetryCancellationPoint);
        }
      }

      EnumerateContentResult ECR(CE.AR->enumerateContent(CE.j, Depth));
      switch (ECR.getResultType()) {
      case EnumerateContentResult::COMPLETE:
        ContentEnumerators.pop_front();
        break;

      case EnumerateContentResult::RETRY: {
        EnumerationState *NewRetryCancellationPoint =
            ECR.getRetryCancellationPoint();
        assert(NewRetryCancellationPoint->isEnumerating());
        assert(NewRetryCancellationPoint->EnumerationDepth <= EnumerationDepth);
        // Overall retry cancellation point is the one with the least depth.
        if (!FirstRetrySource) {
          assert(!RetryCancellationPoint);
          FirstRetrySource = CE.AR;
          RetryCancellationPoint = NewRetryCancellationPoint;
        } else {
          assert(RetryCancellationPoint);
          assert(RetryCancellationPoint->isEnumerating());
          if (NewRetryCancellationPoint->EnumerationDepth <
              RetryCancellationPoint->EnumerationDepth) {
            RetryCancellationPoint = NewRetryCancellationPoint;
          }
        }
        ContentEnumerators.push_back(CE);
        ContentEnumerators.pop_front();
        break;
      }

      case EnumerateContentResult::VALUE_INFO_CONTENT: {
        ++CE.j;
        ValueInfo *VI = ECR.getValueInfoContent();
        Elements.push_back(VI);
        return EnumerateElementsResult::makeElementResult(VI);
      }

      case EnumerateContentResult::SUBSET_CONTENT:
        ++CE.j;
        addSubset(ECR.getSubsetContent());
        break;

      default:
        llvm_unreachable("Not a recognized EnumerateContentResult");
        break;
      }
    } while (!isDone());
  }
  return EnumerateElementsResult::makeCompleteResult();
}

void EnumerationState::writeEquation(const DebugInfo &DI,
    raw_ostream &OS) const {
  OS << ExitCount << " hits: ";
  DI.printEnumerationStateName(this, OS);
  OS << " = ";
  bool first = true;
  if (!Elements.empty()) {
    OS << '{';
    for (ValueInfoVector::const_iterator i = Elements.begin();
         i != Elements.end(); ++i) {
      if (!first) {
        OS << ", ";
      }
      first = false;
      DI.printValueInfoName(*i, OS);
    }
    OS << '}';
  }
  if (!isDone()) {
    if (!first) {
      OS << " U ";
    }
    OS << "enumerate(remaining={";
    first = true;
    for (std::deque<ContentEnumerator>::const_iterator i =
             ContentEnumerators.begin(); i != ContentEnumerators.end(); ++i) {
      if (!first) {
        OS << ", ";
      }
      first = false;
      DI.printAnalysisResultName(i->AR, OS);
      OS << '[' << i->j << ":]";
    }
    OS << "}, visited={";
    first = true;
    for (std::set<AnalysisResult *>::const_iterator i = Subsets.begin();
         i != Subsets.end(); ++i) {
      if (!first) {
        OS << ", ";
      }
      first = false;
      DI.printAnalysisResultName(*i, OS);
    }
    OS << '}';
  }
  OS << '\n';
}

void EnumerationState::addSubset(AnalysisResult *AR) {
  if (Subsets.insert(AR).second) {
    ContentEnumerators.push_front(ContentEnumerator(AR));
  }
}

}
}
