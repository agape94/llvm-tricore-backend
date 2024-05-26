//===-- TriCore.h - Top-level interface for TriCore representation --*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// TriCore back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICORE_H
#define LLVM_LIB_TARGET_TRICORE_TRICORE_H

#include "llvm/Pass.h"

namespace llvm {
class FunctionPass;
class TriCoreTargetMachine;
class PassRegistry;

// createTriCoreISelDag - This pass converts a legalized DAG into a
// TriCore-specific DAG, ready for instruction scheduling.
FunctionPass *createTriCoreISelDag(TriCoreTargetMachine &TM);

// createTriCoreDelaySlotFillerPass - This pass fills delay slots
// with useful instructions or nop's
FunctionPass *createTriCoreDelaySlotFillerPass(const TriCoreTargetMachine &TM);

// createTriCoreMemAluCombinerPass - This pass combines loads/stores and
// arithmetic operations.
FunctionPass *createTriCoreMemAluCombinerPass();

// createTriCoreSetflagAluCombinerPass - This pass combines SET_FLAG and ALU
// operations.
FunctionPass *createTriCoreSetflagAluCombinerPass();

void initializeTriCoreDAGToDAGISelPass(PassRegistry &);

} // namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICORE_H
