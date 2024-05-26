//===-- TriCoreMCAsmInfo.cpp - TriCore asm properties -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the TriCoreMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "TriCoreMCAsmInfo.h"

#include "llvm/TargetParser/Triple.h"

using namespace llvm;

void TriCoreMCAsmInfo::anchor() {}

TriCoreMCAsmInfo::TriCoreMCAsmInfo(const Triple & /*TheTriple*/,
                               const MCTargetOptions &Options) {}
