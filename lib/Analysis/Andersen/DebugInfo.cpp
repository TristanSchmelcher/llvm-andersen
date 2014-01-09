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

std::string prettyPrintValue(const Value *V, size_t MaxPrintedSize) {
  assert(V);
  std::ostringstream OSS;
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
      OSS << PrintedValue << " ...";
    } else {
      OSS << PrintedValue;
    }
  }
  if (V->hasName()) {
    OSS << " (" << V->getName().str() << ')';
  }
  return OSS.str();
}

}

DebugInfo::DebugInfo(const Data *D) : D(D) {
  D->fillDebugInfo(static_cast<DebugInfoFiller *>(this));
}

std::string DebugInfo::getValueInfoName(const ValueInfo *VI) const {
  if (VI->getValue()) {
    static const size_t MaxPrintedSize = 16;
    return prettyPrintValue(VI->getValue(), MaxPrintedSize);
  } else if (VI == D->ExternallyLinkableRegions.getPtr()) {
    return "ExternallyLinkableRegions";
  } else if (VI == D->ExternallyAccessibleRegions.getPtr()) {
    return "ExternallyAccessibleRegions";
  } else {
    std::ostringstream OSS;
    OSS << "Anonymous" << VI;
    return OSS.str();
  }
}

std::string DebugInfo::getAnalysisResultName(const AnalysisResult *AR) const {
  AnalysisResultInfoMap::const_iterator i = ARIM.find(AR);
  assert(i != ARIM.end());
  std::ostringstream OSS;
  OSS << i->second.second->getAlgorithmName() << '('
      << getValueInfoName(i->second.first) << ')';
  return OSS.str();
}

}
}
