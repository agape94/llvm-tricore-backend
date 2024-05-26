//===- TriCoreCallingConv.td - Calling Conventions TriCore -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the TriCore architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Return Value Calling Conventions
//===----------------------------------------------------------------------===//

// TriCore C Calling convention.
def CC_TriCore : CallingConv<[]>;
// TriCore C return-value convention.
def RetCC_TriCore : CallingConv<[]>;
