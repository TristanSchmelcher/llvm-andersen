//===- DebugInfo.cpp - global information for debugging output --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a type for global debugging information.
//
//===----------------------------------------------------------------------===//

#include "DebugInfo.h"

#include "AlgorithmId.h"
#include "Data.h"
#include "ValueInfo.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_os_ostream.h"

#include <sstream>

namespace llvm {
namespace andersen_internal {

namespace {

// Trim functions taken from
// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(
      std::ptr_fun<int, int>(std::isspace))));
  return s;
}

std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(
      std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}

std::string getPrintedValue(const Value *V) {
  std::ostringstream OSS;
  {
    raw_os_ostream OS(OSS);
    V->print(OS);
  }
  return OSS.str();
}

}

DebugInfo::DebugInfo(const Data *D) : D(D) {
  D->fillDebugInfo(static_cast<DebugInfoFiller *>(this));
}

void DebugInfo::printValueName(const Value *V, raw_ostream &OS) {
  OS << V << ':';
  static const size_t MaxPrintedSize = 16;
  {
    std::string PrintedValue(getPrintedValue(V));
    size_t n = PrintedValue.find('\n');
    if (n != std::string::npos) {
      PrintedValue.erase(n);
    }
    trim(PrintedValue);
    if (PrintedValue.size() > MaxPrintedSize) {
      PrintedValue.erase(MaxPrintedSize);
      rtrim(PrintedValue);
      OS << PrintedValue << " ...";
    } else {
      OS << PrintedValue;
    }
  }
  if (V->hasName()) {
    OS << " (" << V->getName().str() << ')';
  }
}

void DebugInfo::printValueInfoName(const ValueInfo *VI, raw_ostream &OS) const {
  if (const Value *V = VI->getValue()) {
    printValueName(V, OS);
  } else if (VI == D->ExternallyLinkableRegions.getPtr()) {
    OS << "ExternallyLinkableRegions";
  } else if (VI == D->ExternallyAccessibleRegions.getPtr()) {
    OS << "ExternallyAccessibleRegions";
  } else {
    OS << "Anonymous" << VI;
  }
}

void DebugInfo::printAnalysisResultName(const AnalysisResult *AR,
                                        raw_ostream &OS) const {
  AnalysisResultInfoMap::const_iterator i = ARIM.find(AR);
  assert(i != ARIM.end());
  i->second.second->printAlgorithmName(OS);
  OS << '(';
  printValueInfoName(i->second.first, OS);
  OS << ')';
}

}
}
