//=====-- TriCoreMCAsmInfo.h - TriCore asm properties -----------*- C++ -*--====//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the TriCoreMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_MCTARGETDESC_TRICOREMCASMINFO_H
#define LLVM_LIB_TARGET_TRICORE_MCTARGETDESC_TRICOREMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class TriCoreMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit TriCoreMCAsmInfo(const Triple &TheTriple,
                          const MCTargetOptions &Options);
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_MCTARGETDESC_TRICOREMCASMINFO_H
