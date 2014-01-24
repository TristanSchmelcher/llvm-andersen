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
#include "EnumerateContentResult.h"
#include "EnumerateElementsResult.h"
#include "llvm/Support/ErrorHandling.h"

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

inline EnumerationState::RetryEntry::RetryEntry(EnumerationState *ES)
  : EnumerationDepth(ES->EnumerationDepth), ES(ES), ExitCount(ES->ExitCount),
    ElementsSize(ES->Elements.size()) {
  assert(ES->isEnumerating());
}

inline bool EnumerationState::RetryEntryComparator::operator()(
    const RetryEntry &E1, const RetryEntry &E2) const {
  assert((E1.EnumerationDepth != E2.EnumerationDepth) ||
         (E1.ES == E2.ES && E1.ExitCount == E2.ExitCount));
  return E1.EnumerationDepth > E2.EnumerationDepth;
}

EnumerationState::EnumerationState(AnalysisResult *AR)
  : EnumerationDepth(-1), ExitCount(0) {
  addSubset(AR);
}

inline bool EnumerationState::haveCachedRetryResult() {
  if (RS.empty()) {
    // No cached retry.
    return false;
  }

  // Last enumeration attempt returned retry. See if the state is unchanged.
  RetryState::const_iterator i;
  for (i = RS.begin(); i != RS.end(); ++i) {
    const RetryEntry &RE(*i);
    if (RE.ExitCount == RE.ES->ExitCount) {
      // The corresponding enumerateElements stack frame has not yet
      // unwound, so this and everything after is in the same state.
      break;
    }
    if (!RE.ES->isEnumerating() ||
        RE.ElementsSize != RE.ES->Elements.size()) {
      // The retry state for this has changed, so we need to enumerate it.
      RS.clear();
      return false;
    }
    // Otherwise the stack has unwound but enumeration has since re-entered this
    // and no new elements were enumerated in the meantime.
  }

  // Remove and re-add all the modified things.
  std::vector<EnumerationState *> modified;
  for (RetryState::const_iterator j = RS.begin(); j != i; ++j) {
    const RetryEntry &RE(*i);
    modified.push_back(RE.ES);
  }
  RS.erase(RS.begin(), i);
  RS.insert(modified.begin(), modified.end());

  return true;
}

EnumerateElementsResult EnumerationState::enumerateElements(size_t i,
    int Depth) {
  assert(i <= Elements.size());
  if (i < Elements.size()) {
    return EnumerateElementsResult::makeElementResult(Elements[i]);
  }
  if (!isDone()) {
    if (isEnumerating()) {
      return EnumerateElementsResult::makeRetryStartResult(this);
    }
    if (haveCachedRetryResult()) {
      return EnumerateElementsResult::makeRetryContinueResult(this);
    }
    ScopedEnumerateContext Enumerating(this, Depth++);
    AnalysisResult *FirstRetrySource = 0;
    do {
      ContentEnumerator &CE(ContentEnumerators.front());
      if (CE.AR == FirstRetrySource) {
        // We've looped around and not found any new elements.
        if (RS.empty()) {
          // We have self-contained reference cycles and this iteration found
          // no new items. Therefore we're done. Cancel the retry state.
          ContentEnumerators.clear();
          break;
        } else {
          // We have non-self-contained reference cycles. Must retry
          // enumeration later.
          return EnumerateElementsResult::makeRetryContinueResult(this);
        }
      }

      EnumerateContentResult ECR(CE.AR->enumerateContent(CE.j, Depth));
      switch (ECR.getResultType()) {
      case EnumerateContentResult::COMPLETE:
        ContentEnumerators.pop_front();
        break;

      case EnumerateContentResult::RETRY_START: {
        if (!FirstRetrySource) {
          FirstRetrySource = CE.AR;
        }
        EnumerationState *RetryPoint = ECR.getRetrySource();
        assert(RetryPoint->isEnumerating());
        if (RetryPoint != this) {
          RS.insert(RetryPoint);
        }
        // Skip it and move it to the back.
        ContentEnumerators.push_back(CE);
        ContentEnumerators.pop_front();
        break;
      }

      case EnumerateContentResult::RETRY_CONTINUE: {
        if (!FirstRetrySource) {
          FirstRetrySource = CE.AR;
        }
        const RetryState &NewRS(ECR.getRetrySource()->RS);
        for (RetryState::const_iterator i = NewRS.begin(); i != NewRS.end();
             ++i) {
          const RetryEntry &RE(*i);
          if (RE.ES != this) {
            RS.insert(RE);
          }
        }
        // Skip it and move it to the back.
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

void EnumerationState::addSubset(AnalysisResult *AR) {
  if (Subsets.insert(AR).second) {
    ContentEnumerators.push_front(ContentEnumerator(AR));
  }
}

}
}
