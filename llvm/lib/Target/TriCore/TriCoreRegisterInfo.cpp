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

TriCoreRegisterInfo::TriCoreRegisterInfo() : TriCoreGenRegisterInfo(TRICORE::R0) {}

const uint16_t *
TriCoreRegisterInfo::getCalleeSavedRegs(const MachineFunction * /*MF*/) const {
  static const uint16_t CalleeSavedRegs[] = {
    TRICORE::R4,
    0
  };
  return CalleeSavedRegs;
}

BitVector TriCoreRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(TRICORE::R8);
  // Add more registers that should be reserved
  return Reserved;
}

bool TriCoreRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  // true indicates that we changed the instruction.
  return true;
}

Register
TriCoreRegisterInfo::getFrameRegister(const MachineFunction & /*MF*/) const {
  return TRICORE::R10;
}
