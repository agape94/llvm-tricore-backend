//===-- TriCoreFrameLowering.h - Define frame lowering for TriCore --*- C++-*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class implements TriCore-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICOREFRAMELOWERING_H
#define LLVM_LIB_TARGET_TRICORE_TRICOREFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {

class BitVector;
class TriCoreSubtarget;

class TriCoreFrameLowering : public TargetFrameLowering {
private:
  void determineFrameLayout(MachineFunction &MF) const;
  void replaceAdjDynAllocPseudo(MachineFunction &MF) const;
  uint64_t computeStackSize(MachineFunction &MF) const;

protected:
  const TriCoreSubtarget &STI;

public:
  explicit TriCoreFrameLowering(const TriCoreSubtarget &Subtarget)
      : TargetFrameLowering(StackGrowsDown,
                            /*StackAlignment=*/Align(8),
                            /*LocalAreaOffset=*/0),
        STI(Subtarget) {}
  
  //! Stack slot size (4 bytes)
  static int stackSlotSize() {
    return 4;
  }

  // emitProlog/emitEpilog - These methods insert prolog and epilog code into
  // the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  bool hasFP(const MachineFunction &MF) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS = nullptr) const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICOREFRAMELOWERING_H
