//===-- TriCoreTargetObjectFile.h - TriCore Object Info -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICORETARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_TRICORE_TRICORETARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class TriCoreTargetObjectFile : public TargetLoweringObjectFileELF {};
} // end namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICORETARGETOBJECTFILE_H
