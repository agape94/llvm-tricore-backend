//===--- TriCore.cpp - Implement TriCore target feature support ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements TriCore TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#include "TriCore.h"
#include "clang/Basic/MacroBuilder.h"
#include "llvm/ADT/StringSwitch.h"

using namespace clang;
using namespace clang::targets;

const char *const TriCoreTargetInfo::GCCRegNames[] = {
    "d0",  "d1",  "d2",  "d3",  "d4",  "d5",  "d6",  "d7",  "d8",  "d9",  "d10",
    "d11", "d12", "d13", "d14", "d15", "a0",  "a1",  "a2",  "a3",  "a4",  "a5",  "a6",  "a7",  "a8",  "a9",  "a10",
    "a11", "a12", "a13", "a14", "a15", "e0", "e2", "e4", "e6", "e8", "e10", "e12", "e14", 
    "psw", "pcxi", "pc", "fcx"
};

ArrayRef<const char *> TriCoreTargetInfo::getGCCRegNames() const {
  return llvm::ArrayRef(GCCRegNames);
}

const TargetInfo::GCCRegAlias TriCoreTargetInfo::GCCRegAliases[] = {
    {{"pc"}, "pc"},   {{"sp"}, "a10"},   {{"fp"}, "a14"},   {{"rv"}, "d2"},
    // {{"rr1"}, "r10"}, {{"rr2"}, "r11"}, {{"rca"}, "r15"},
};

ArrayRef<TargetInfo::GCCRegAlias> TriCoreTargetInfo::getGCCRegAliases() const {
  return llvm::ArrayRef(GCCRegAliases);
}

void TriCoreTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  Builder.defineMacro("tricore");
  Builder.defineMacro("__TRICORE__");
}

