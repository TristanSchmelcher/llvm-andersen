//===- LazyAndersenRelationsGraphViewer.h - graph viewing -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares a function to display a graph of the relations.
//
//===----------------------------------------------------------------------===//

#ifndef LAZYANDERSENRELATIONSGRAPHVIEWER_H
#define LAZYANDERSENRELATIONSGRAPHVIEWER_H

namespace llvm {
  class LazyAndersenData;
  class Module;
}

namespace llvm {
namespace lazyandersen {
  void viewRelationsGraph(LazyAndersenData *Data, const Module *M = 0);
}
}

#endif
