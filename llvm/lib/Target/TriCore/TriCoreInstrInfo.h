//===- TriCoreInstrInfo.h - TriCore Instruction Information ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the TriCore implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICOREINSTRINFO_H
#define LLVM_LIB_TARGET_TRICORE_TRICOREINSTRINFO_H

#include "TriCoreRegisterInfo.h"
#include "MCTargetDesc/TriCoreMCTargetDesc.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "TriCoreGenInstrInfo.inc"

namespace llvm {

namespace TriCoreCC {
  enum CondCodes {
    COND_EQ, // Equal
    COND_NE, // Not equal
    COND_GE, // Greater than or equal
    COND_LT, // Less than
    COND_INVALID
  };

  enum LogicCodes {
      LOGIC_AND, // AND
      LOGIC_OR,  // OR
      LOGIC_INVALID
    };
}
class TriCoreInstrInfo : public TriCoreGenInstrInfo {
  const TriCoreRegisterInfo RegisterInfo;

public:
  TriCoreInstrInfo();

  // getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  // such, whenever a client has an instance of instruction info, it should
  // always be able to get register info as well (through this method).
  virtual const TriCoreRegisterInfo &getRegisterInfo() const {
    return RegisterInfo;
  }
  void splitRegs(unsigned Reg, unsigned &LoReg, unsigned &HiReg) const;
  bool expandPostRAPseudo(MachineInstr &MI) const override;
  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc) const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICOREINSTRINFO_H
