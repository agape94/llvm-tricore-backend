//===-- TriCoreInstrInfo.cpp - TriCore Instruction Information ------*- C++ -*-===//
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

#include "TriCoreInstrInfo.h"
#include "TriCoreAluCode.h"
#include "MCTargetDesc/TriCoreBaseInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "TriCoreGenInstrInfo.inc"

TriCoreInstrInfo::TriCoreInstrInfo()
    : TriCoreGenInstrInfo(TRICORE::ADJCALLSTACKDOWN, TRICORE::ADJCALLSTACKUP),
      RegisterInfo() {}

/// Splits a register into a lo:hi register pair.
void TriCoreInstrInfo::splitRegs(unsigned Reg, unsigned &LoReg, unsigned &HiReg) const {
  LoReg = RegisterInfo.getSubReg(Reg, TRICORE::subreg_even);
  HiReg = RegisterInfo.getSubReg(Reg, TRICORE::subreg_odd);
}

bool TriCoreInstrInfo::expandPostRAPseudo(MachineInstr &MI) const
{
DebugLoc DL = MI.getDebugLoc();
  MachineBasicBlock &MBB = *(MI.getParent());

  switch (MI.getOpcode())
  {
  default:
    return false;
  case TRICORE::NOTrr64: {
    unsigned OpLo, OpHi, DstLoReg, DstHiReg;

    unsigned DstReg = MI.getOperand(0).getReg();

    bool DstIsDead = MI.getOperand(0).isDead();
    bool DstIsKill = MI.getOperand(1).isKill();

    splitRegs(DstReg, DstLoReg, DstHiReg);

    OpLo = TRICORE::NOTsr;
    OpHi = TRICORE::NOTsr;

    BuildMI(MBB, MI, DL, get(OpLo))
      .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
      .addReg(DstLoReg, getKillRegState(DstIsKill));

    BuildMI(MBB, MI, DL, get(OpHi))
      .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
      .addReg(DstHiReg, getKillRegState(DstIsKill));

    MBB.erase(MI);
    return true;
  }
  case TRICORE::ANDsrr64:
  case TRICORE::XORsrr64:
  case TRICORE::ORsrr64:{
    unsigned OpLo, OpHi, Src0LoReg, Src0HiReg,
              Src1LoReg, Src1HiReg, DstLoReg, DstHiReg;
    unsigned DstReg = MI.getOperand(0).getReg();
    unsigned Src0Reg = MI.getOperand(1).getReg();
    unsigned Src1Reg = MI.getOperand(2).getReg();

    bool DstIsDead = MI.getOperand(0).isDead();
    bool Src0IsKill = MI.getOperand(1).isKill();
    bool Src1IsKill = MI.getOperand(2).isKill();

    splitRegs(Src0Reg, Src0LoReg, Src0HiReg);
    splitRegs(Src1Reg, Src1LoReg, Src1HiReg);
    splitRegs(DstReg, DstLoReg, DstHiReg);

    if (MI.getOpcode() == TRICORE::ANDsrr64) {
        OpLo = TRICORE::ANDsrr;
        OpHi = TRICORE::ANDsrr;
    }
    else if (MI.getOpcode() == TRICORE::XORsrr64) {
      OpLo = TRICORE::XORsrr;
      OpHi = TRICORE::XORsrr;
    }
    else {
        OpLo = TRICORE::ORsrr;
        OpHi = TRICORE::ORsrr;
    }

    BuildMI(MBB, MI, DL, get(OpLo))
      .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
      .addReg(Src0LoReg, getKillRegState(Src0IsKill))
      .addReg(Src1LoReg, getKillRegState(Src1IsKill));

    BuildMI(MBB, MI, DL, get(OpHi))
      .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
      .addReg(Src0HiReg, getKillRegState(Src0IsKill))
      .addReg(Src1HiReg, getKillRegState(Src1IsKill));

    MBB.erase(MI);
    return true;
  }
  case TRICORE::XORrc64:
  case TRICORE::ANDrc64:
  case TRICORE::ORrc64:
  case TRICORE::ANDNrc64:
  case TRICORE::ORNrc64:
  case TRICORE::XORrcneg64:
  {
      unsigned OpLo, OpHi, Src0LoReg, Src0HiReg,
      DstLoReg, DstHiReg;

      unsigned DstReg = MI.getOperand(0).getReg();
      unsigned Src0Reg = MI.getOperand(1).getReg();

      bool DstIsDead = MI.getOperand(0).isDead();
      bool Src0IsKill = MI.getOperand(1).isKill();

      int64_t immVal = MI.getOperand(2).getImm();
      int32_t lowByte = immVal & 0xffffffff;
      int32_t highByte = (immVal>>32) & 0xffffffff;

      splitRegs(Src0Reg, Src0LoReg, Src0HiReg);
      splitRegs(DstReg, DstLoReg, DstHiReg);

      if (MI.getOpcode() == TRICORE::ANDrc64) {
        OpLo = TRICORE::ANDrc;
        OpHi = TRICORE::ANDrc;
      }
      else if (MI.getOpcode() == TRICORE::XORrc64) {
        OpLo = TRICORE::XORrc;
        OpHi = TRICORE::XORrc;
      }
      else if (MI.getOpcode() == TRICORE::XORrcneg64) {
        OpLo = TRICORE::XNORrc;
        OpHi = TRICORE::XNORrc;
      }
      else if (MI.getOpcode() == TRICORE::ORNrc64) {
        OpLo = TRICORE::ORNrc;
        OpHi = TRICORE::ORNrc;
      }
      else if(MI.getOpcode() == TRICORE::ANDNrc64) {
        OpLo = TRICORE::ANDNrc;
        OpHi = TRICORE::ANDNrc;
      }
      else {
        OpLo = TRICORE::ORrc;
        OpHi = TRICORE::ORrc;
      }

      BuildMI(MBB, MI, DL, get(OpLo))
        .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
        .addReg(Src0LoReg, getKillRegState(Src0IsKill))
        .addImm(lowByte);

      BuildMI(MBB, MI, DL, get(OpHi))
          .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
          .addReg(Src0HiReg, getKillRegState(Src0IsKill))
          .addImm(highByte);

      MBB.erase(MI);
      return true;
    }
  case TRICORE::ADDi64:
  case TRICORE::SUBi64:{

    unsigned OpLo, OpHi, Src0LoReg, Src0HiReg,
              Src1LoReg, Src1HiReg, DstLoReg, DstHiReg;
    unsigned DstReg = MI.getOperand(0).getReg();
    unsigned Src0Reg = MI.getOperand(1).getReg();
    unsigned Src1Reg = MI.getOperand(2).getReg();

    bool DstIsDead = MI.getOperand(0).isDead();
    bool Src0IsKill = MI.getOperand(1).isKill();
    bool Src1IsKill = MI.getOperand(2).isKill();
    bool ImpIsDead = MI.getOperand(3).isDead();

    splitRegs(Src0Reg, Src0LoReg, Src0HiReg);
    splitRegs(Src1Reg, Src1LoReg, Src1HiReg);
    splitRegs(DstReg, DstLoReg, DstHiReg);

    if (MI.getOpcode() == TRICORE::ADDi64) {
      OpLo = TRICORE::ADDXrr;
      OpHi = TRICORE::ADDCrr;
    }
    else {
      OpLo = TRICORE::SUBXrr;
      OpHi = TRICORE::SUBCrr;
    }

    auto MIBLO =
          BuildMI(MBB, MI, DL, get(OpLo))
              .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
              .addReg(Src0LoReg, getKillRegState(Src0IsKill))
              .addReg(Src1LoReg, getKillRegState(Src1IsKill));

    //PSW is implicitly killed
    MIBLO->getOperand(4).setIsKill();

    auto MIBHI =
              BuildMI(MBB, MI, DL, get(OpHi))
                  .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
                  .addReg(Src0HiReg, getKillRegState(Src0IsKill))
                  .addReg(Src1HiReg, getKillRegState(Src1IsKill));

    if (ImpIsDead)
        MIBHI->getOperand(3).setIsDead();

    // PSW is always implicitly killed
    MIBHI->getOperand(4).setIsKill();
    MBB.erase(MI);
    return true;
  }
  case TRICORE::ADDi64C: {
    unsigned OpLo, OpHi, Src0LoReg, Src0HiReg,
    DstLoReg, DstHiReg;

    unsigned DstReg = MI.getOperand(0).getReg();
    unsigned Src0Reg = MI.getOperand(1).getReg();

    bool DstIsDead = MI.getOperand(0).isDead();
    bool Src0IsKill = MI.getOperand(1).isKill();
    bool ImpIsDead = MI.getOperand(3).isDead();

    int64_t immVal = MI.getOperand(2).getImm();
    int32_t lowByte = immVal & 0xffffffff;
    int32_t highByte = (immVal>>32) & 0xffffffff;

    splitRegs(Src0Reg, Src0LoReg, Src0HiReg);
    splitRegs(DstReg, DstLoReg, DstHiReg);

    OpLo = TRICORE::ADDXrc;
    OpHi = TRICORE::ADDCrc;

    auto MIBLO =
        BuildMI(MBB, MI, DL, get(OpLo))
        .addReg(DstLoReg, RegState::Define | getDeadRegState(DstIsDead))
        .addReg(Src0LoReg, getKillRegState(Src0IsKill))
        .addImm(lowByte);

    //PSW is implicitly killed
    MIBLO->getOperand(4).setIsKill();

    auto MIBHI =
        BuildMI(MBB, MI, DL, get(OpHi))
        .addReg(DstHiReg, RegState::Define | getDeadRegState(DstIsDead))
        .addReg(Src0HiReg, getKillRegState(Src0IsKill))
        .addImm(highByte);

    if (ImpIsDead)
      MIBHI->getOperand(3).setIsDead();

    // PSW is always implicitly killed
    MIBHI->getOperand(4).setIsKill();

    MBB.erase(MI);
    return true;

  }
  case TRICORE::MOVi32: {

    const unsigned DstReg = MI.getOperand(0).getReg();
    const bool DstIsDead = MI.getOperand(0).isDead();

    const MachineOperand &MO = MI.getOperand(1);
    if (MO.isImm()) {

      int64_t ImmVal = MO.getImm();

      // Select the low part of the immediate move.
      int64_t LoMask = 0xffff;
      int64_t ImmLo = (ImmVal & LoMask);
      int64_t ImmLo_ext64 = (int16_t)ImmLo;
      int64_t hiShift = (ImmVal - ImmLo_ext64) >> 16;

      if (hiShift < 0)
          hiShift = 65536 + hiShift;

      const int64_t Imm = MO.getImm();
      const int64_t Lo16 = Imm & 0xffff;
      //const unsigned Hi16 = (Imm >> 16) & 0xffff;
      //HI16 = HI16.addImm(hiShift);

      if((hiShift == 0) && (Lo16 < 65536))
        BuildMI(MBB, MI, DL, get(TRICORE::MOVrlc))
             .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
             .addImm(Lo16);
      else if(!(hiShift == 0))
        BuildMI(MBB, MI, DL, get(TRICORE::MOVHrlc))
              .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
              .addImm(hiShift);

      if ((hiShift != 0) && Lo16)
        BuildMI(MBB, MI, DL, get(TRICORE::ADDIrlc))
           .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
           .addReg(DstReg)
           .addImm(ImmLo_ext64);
    }
    else {
      const GlobalValue *GV = MO.getGlobal();
      const unsigned TF = MO.getTargetFlags();
      auto HI16 = BuildMI(MBB, MI, DL, get(TRICORE::MOVHrlc))
                .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead));
      HI16 = HI16.addGlobalAddress(GV, MO.getOffset() ,  TF  | TriCoreII::MO_HI_OFFSET);

      auto ADDIrlc = BuildMI(MBB, MI, DL, get(TRICORE::ADDIrlc))
                     .addReg(DstReg, RegState::Define | getDeadRegState(DstIsDead))
                     .addReg(DstReg);
      ADDIrlc = ADDIrlc.addGlobalAddress(GV,MO.getOffset() , TF  | TriCoreII::MO_LO_OFFSET);
    }

    MBB.erase(MI);
    return true;
  }
  }
}
void TriCoreInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I,
                                  const DebugLoc &DL, MCRegister DestReg,
                                  MCRegister SrcReg, bool KillSrc) const {
  unsigned Opc;
  if (TRICORE::DataRegsRegClass.contains(DestReg, SrcReg))
    Opc = TRICORE::MOVrr;
  else if (TRICORE::AddrRegsRegClass.contains(DestReg, SrcReg))
    Opc = TRICORE::MOV_AArr;
  else if (TRICORE::AddrRegsRegClass.contains(DestReg) && TRICORE::DataRegsRegClass.contains(SrcReg))
    Opc = TRICORE::MOV_Arr;
  else if (TRICORE::AddrRegsRegClass.contains(SrcReg) && TRICORE::DataRegsRegClass.contains(DestReg))
    Opc = TRICORE::MOV_Drr;
  else
    llvm_unreachable("Impossible reg-to-reg copy");

  BuildMI(MBB, I, DL, get(Opc), DestReg)
    .addReg(SrcReg, getKillRegState(KillSrc));
}