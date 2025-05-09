//===-- TriCoreTargetInfo.cpp - TriCore Target Implementation -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/TriCoreTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheTriCoreTarget() {
  static Target TheTriCoreTarget;
  return TheTriCoreTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTriCoreTargetInfo() {
  RegisterTarget<Triple::tricore> X(getTheTriCoreTarget(), "tricore", "TriCore",
                                  "TriCore");
}
