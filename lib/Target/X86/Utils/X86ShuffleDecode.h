//===-- X86ShuffleDecode.h - X86 shuffle decode logic -----------*-C++-*---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Define several functions to decode x86 specific shuffle semantics into a
// generic vector mask.
//
//===----------------------------------------------------------------------===//

#ifndef X86_SHUFFLE_DECODE_H
#define X86_SHUFFLE_DECODE_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/ValueTypes.h"

//===----------------------------------------------------------------------===//
//  Vector Mask Decoding
//===----------------------------------------------------------------------===//

namespace llvm {
enum {
  SM_SentinelZero = ~0U
};

void DecodeINSERTPSMask(unsigned Imm, SmallVectorImpl<unsigned> &ShuffleMask);

// <3,1> or <6,7,2,3>
void DecodeMOVHLPSMask(unsigned NElts,
                       SmallVectorImpl<unsigned> &ShuffleMask);

// <0,2> or <0,1,4,5>
void DecodeMOVLHPSMask(unsigned NElts,
                       SmallVectorImpl<unsigned> &ShuffleMask);

void DecodePSHUFMask(EVT VT, unsigned Imm,
                     SmallVectorImpl<unsigned> &ShuffleMask);

void DecodePSHUFHWMask(unsigned Imm,
                       SmallVectorImpl<unsigned> &ShuffleMask);

void DecodePSHUFLWMask(unsigned Imm,
                       SmallVectorImpl<unsigned> &ShuffleMask);

/// DecodeSHUFPMask - This decodes the shuffle masks for shufp*. VT indicates
/// the type of the vector allowing it to handle different datatypes and vector
/// widths.
void DecodeSHUFPMask(EVT VT, unsigned Imm,
                     SmallVectorImpl<unsigned> &ShuffleMask);

/// DecodeUNPCKHMask - This decodes the shuffle masks for unpckhps/unpckhpd
/// and punpckh*. VT indicates the type of the vector allowing it to handle
/// different datatypes and vector widths.
void DecodeUNPCKHMask(EVT VT, SmallVectorImpl<unsigned> &ShuffleMask);

/// DecodeUNPCKLMask - This decodes the shuffle masks for unpcklps/unpcklpd
/// and punpckl*. VT indicates the type of the vector allowing it to handle
/// different datatypes and vector widths.
void DecodeUNPCKLMask(EVT VT, SmallVectorImpl<unsigned> &ShuffleMask);


void DecodeVPERM2X128Mask(EVT VT, unsigned Imm,
                          SmallVectorImpl<unsigned> &ShuffleMask);

} // llvm namespace

#endif
