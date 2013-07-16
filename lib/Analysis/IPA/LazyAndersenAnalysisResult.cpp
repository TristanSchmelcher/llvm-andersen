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

#include "LazyAndersenAnalysisResult-inl.h"

using namespace llvm;
using namespace llvm::lazyandersen;

AnalysisResult::AnalysisResult() : Enumerating(false) {}

AnalysisResult::~AnalysisResult() {}
