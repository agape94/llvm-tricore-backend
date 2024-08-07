//===-- TriCoreRegisterInfo.cpp - TriCore Register Information ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the TriCore implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "TriCoreRegisterInfo.h"
#include "TriCoreAluCode.h"
#include "TriCoreFrameLowering.h"
#include "TriCoreInstrInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "TriCoreGenRegisterInfo.inc"

using namespace llvm;

TriCoreRegisterInfo::TriCoreRegisterInfo() : TriCoreGenRegisterInfo(TRICORE::A11) {}

const uint16_t *
TriCoreRegisterInfo::getCalleeSavedRegs(const MachineFunction * /*MF*/) const {
  static const MCPhysReg CalleeSavedRegs[] = { 0 };  
  return CalleeSavedRegs;
}

BitVector TriCoreRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
    BitVector Reserved(getNumRegs());

  Reserved.set(TRICORE::PC);
  Reserved.set(TRICORE::PCXI);
  Reserved.set(TRICORE::A10);
  Reserved.set(TRICORE::A11);
  Reserved.set(TRICORE::PSW);
  Reserved.set(TRICORE::FCX);
  return Reserved;
}

const uint32_t *TriCoreRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                                      CallingConv::ID) const {
  return CC_Save_RegMask;
}

bool
TriCoreRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
TriCoreRegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

bool TriCoreRegisterInfo::useFPForScavengingIndex(const MachineFunction &MF) const {
  return false;
}

bool TriCoreRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  outs() << "TriCoreRegisterInfo::eliminateFrameIndex\n";
  assert(SPAdj == 0 && "Unexpected");
  MachineInstr &MI = *II;
  const MachineFunction &MF = *MI.getParent()->getParent();
  DebugLoc dl = MI.getDebugLoc();
  MachineBasicBlock &MBB = *MI.getParent();
  const auto& MFI = MF.getFrameInfo();
  MachineOperand &FIOp = MI.getOperand(FIOperandNum);
  unsigned FI = FIOp.getIndex();
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  unsigned BasePtr = (TFI->hasFP(MF) ? TRICORE::A14 : TRICORE::A10);
  // Determine if we can eliminate the index from this kind of instruction.
  unsigned ImmOpIdx = 0;

  if (MI.getOpcode() == TRICORE::ADDrc) {    
    int Offset = MFI.getObjectOffset(FI);    
    Offset = -Offset;
    const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();
    MI.setDesc(TII.get(TRICORE::MOV_Drr));
    MI.getOperand(FIOperandNum).ChangeToRegister(BasePtr, false);

    if (Offset == 0)
      return false;

    // We need to materialize the offset via add instruction.
    unsigned DstReg = MI.getOperand(0).getReg();
    if (Offset < 0) {
      BuildMI(MBB, std::next(II), dl, TII.get(TRICORE::ADDrc), DstReg).addReg(
          DstReg).addImm(Offset);
    } else
      BuildMI(MBB, std::next(II), dl, TII.get(TRICORE::ADDrc), DstReg).addReg(
          DstReg).addImm(-Offset);

    return true;
  }

  ImmOpIdx = FIOperandNum + 1;

  // FIXME: check the size of offset.
  MachineOperand &ImmOp = MI.getOperand(ImmOpIdx);
  int Offset = MFI.getObjectOffset(FI);
  FIOp.ChangeToRegister(BasePtr, false);
  ImmOp.setImm(Offset);
  return false;
}

Register
TriCoreRegisterInfo::getFrameRegister(const MachineFunction & MF) const {
  const TriCoreFrameLowering *TFI = getFrameLowering(MF);
  return TFI->hasFP(MF) ? TRICORE::A14 : TRICORE::A10;
}
