//===-- TriCoreFrameLowering.cpp - TriCore Frame Information ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the TriCore implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "TriCoreFrameLowering.h"

#include "TriCoreAluCode.h"
#include "TriCoreInstrInfo.h"
#include "TriCoreSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

// Materialize an offset for a ADD/SUB stack operation.
// Return zero if the offset fits into the instruction as an immediate,
// or the number of the register where the offset is materialized.
static unsigned materializeOffset(MachineFunction &MF, MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MBBI,
                                  unsigned Offset) {
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  const uint64_t MaxSubImm = 0xfff;

  if (Offset <= MaxSubImm) {
    // The stack offset fits in the ADD/SUB instruction.
    return 0;
  } else {
    // The stack offset does not fit in the ADD/SUB instruction.
    // Materialize the offset using MOVLO/MOVHI.
    // FIXME: See to this code, in case we ever get a very large stack.
    //        I guess it should create an error someday.
    unsigned OffsetReg = TRICORE::A14;
    unsigned OffsetLo = (unsigned)(Offset & 0xffff);
    unsigned OffsetHi = (unsigned)((Offset & 0xffff0000) >> 16);
    BuildMI(MBB, MBBI, dl, TII.get(TRICORE::MOVrlc), OffsetReg) //MOVLOi16
        .addImm(OffsetLo)
        .setMIFlag(MachineInstr::FrameSetup);
    if (OffsetHi) {
      BuildMI(MBB, MBBI, dl, TII.get(TRICORE::MOVHrlc), OffsetReg)
          .addReg(OffsetReg)
          .addImm(OffsetHi)
          .setMIFlag(MachineInstr::FrameSetup);
    }
    return OffsetReg;
  }
}

bool TriCoreFrameLowering::hasFP(const MachineFunction &MF) const {

  const MachineFrameInfo &MFI = MF.getFrameInfo();

  return (MF.getTarget().Options.DisableFramePointerElim(MF) ||
         MF.getFrameInfo().hasVarSizedObjects() ||
         MFI.isFrameAddressTaken());
}

uint64_t TriCoreFrameLowering::computeStackSize(MachineFunction &MF) const {
  MachineFrameInfo &MFI = MF.getFrameInfo();
  uint64_t StackSize = MFI.getStackSize();
  unsigned StackAlign = getStackAlignment();
  if (StackAlign > 0) {
    StackSize = alignTo(StackSize, StackAlign);
  }
  return StackSize;
}

void TriCoreFrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const 
{
  // Compute the stack size, to determine if we need a prologue at all.
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  // const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  uint64_t StackSize = computeStackSize(MF);
  if (!StackSize) {
    return;
  }

  if (hasFP(MF)) {
    MachineFunction::iterator I;
    BuildMI(MBB, MBBI, dl, TII.get(TRICORE::MOV_AAsrr), TRICORE::A14)
              .addReg(TRICORE::A10);

    // Mark the FramePtr as live-in in every block except the entry
     for (I = std::next(MF.begin());  I != MF.end(); ++I)
       I->addLiveIn(TRICORE::A14);
  }

  // Adjust the stack pointer.
  unsigned StackReg = TRICORE::A10;
  unsigned OffsetReg = materializeOffset(MF, MBB, MBBI, (unsigned)StackSize);
  if (OffsetReg) {
    BuildMI(MBB, MBBI, dl, TII.get(TRICORE::SUB_Arr), StackReg)
        .addReg(StackReg)
        .addReg(OffsetReg)
        .setMIFlag(MachineInstr::FrameSetup);
  } else {
    BuildMI(MBB, MBBI, dl, TII.get(TRICORE::SUB_Asc))
        .addImm(StackSize)
        .setMIFlag(MachineInstr::FrameSetup);
  }
}

// This function eliminates ADJCALLSTACKDOWN, ADJCALLSTACKUP pseudo
// instructions
MachineBasicBlock::iterator TriCoreFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction & /*MF*/, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
  if (I->getOpcode() == TRICORE::ADJCALLSTACKUP ||
      I->getOpcode() == TRICORE::ADJCALLSTACKDOWN) {
    return MBB.erase(I);
  }
  return I;
}

void TriCoreFrameLowering::emitEpilogue(MachineFunction & /*MF*/,
                                      MachineBasicBlock &MBB) const {}

void TriCoreFrameLowering::determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs, RegScavenger *RS) const {}

