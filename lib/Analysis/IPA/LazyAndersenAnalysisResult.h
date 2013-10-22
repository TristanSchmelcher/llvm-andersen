//===- LazyAndersenAnalysisResult.h - algorithm classes -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a type for analysis results.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENANALYSISRESULT_H
#define LAZYANDERSENANALYSISRESULT_H

#include "LazyAndersenAnalysisResultEntryBase.h"
#include "LazyAndersenGraphNode.h"
#include "llvm/ADT/ilist.h"
#include "llvm/ADT/SetVector.h"

#include <cassert>

namespace llvm {
namespace lazyandersen {
  class ValueInfo;

  class AnalysisResult : public GraphNode {
    int EnumerationDepth;
    typedef SetVector<ValueInfo *> ValueInfoSetVector;
    ValueInfoSetVector Set;
    typedef iplist<AnalysisResultEntryBase> AnalysisResultEntryBaseList;
    AnalysisResultEntryBaseList Work;

  public:
    class EnumerationResult {
    public:
      enum Type {
        NEXT_VALUE,
        RETRY,
        COMPLETE
      };

    private:
      Type type;
      union {
        ValueInfo *NextValue;
        AnalysisResult *CancellationPoint;
        void *Unused;
      };

      explicit EnumerationResult(ValueInfo *NextValue)
        : type(NEXT_VALUE), NextValue(NextValue) {}
      explicit EnumerationResult(AnalysisResult *CancellationPoint)
        : type(RETRY), CancellationPoint(CancellationPoint) {}
      EnumerationResult()
        : type(COMPLETE), Unused(0) {}

    public:
      static EnumerationResult makeNextValueResult(ValueInfo *NextValue) {
        return EnumerationResult(NextValue);
      }

      static EnumerationResult makeRetryResult(
          AnalysisResult *CancellationPoint) {
        return EnumerationResult(CancellationPoint);
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

      AnalysisResult *getRetryCancellationPoint() const {
        assert(type == RETRY);
        return CancellationPoint;
      }
    };

    class Enumerator {
      AnalysisResult *AR;
      ValueInfoSetVector::size_type i;

      class ScopedSetEnumerating {
      protected:
        AnalysisResult *const AR;

      public:
        ScopedSetEnumerating(AnalysisResult *AR, int Depth);
        ~ScopedSetEnumerating();
      };

    public:
      class Context : private ScopedSetEnumerating {
        friend class Enumerator;
        const int NextDepth;
        AnalysisResultEntryBaseList::iterator Pos;

        Context(AnalysisResult *AR, int Depth);

      public:
        int getNextDepth() const { return NextDepth; }

        EnumerationResult pushWork(AnalysisResult *Child);
      };

      explicit Enumerator(AnalysisResult *AR) : AR(AR), i(0) {}

      EnumerationResult enumerate(int Depth);

      GraphEdge toGraphEdge() const;
    };

    AnalysisResult();
    virtual ~AnalysisResult();

    bool addValueInfo(ValueInfo *VI) { return Set.insert(VI); }
    void addWork(AnalysisResultEntryBase *Entry) { Work.push_back(Entry); }

    virtual GraphEdgeDeque getOutgoingEdges() const;
    virtual std::string getNodeLabel() const;
    virtual bool isNodeHidden() const;

  private:
    bool isEnumerating() const { return EnumerationDepth >= 0; }
  };
}
}

#endif
