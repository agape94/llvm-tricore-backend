//===- TriCoreDisassembler.cpp - Disassembler for TriCore -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is part of the TriCore Disassembler.
//
//===----------------------------------------------------------------------===//

#include "TriCoreDisassembler.h"

#include "TriCoreAluCode.h"
#include "TriCoreInstrInfo.h"
#include "TargetInfo/TriCoreTargetInfo.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

typedef MCDisassembler::DecodeStatus DecodeStatus;

static MCDisassembler *createTriCoreDisassembler(const Target & /*T*/,
                                               const MCSubtargetInfo &STI,
                                               MCContext &Ctx) {
  return new TriCoreDisassembler(STI, Ctx);
}

template <int width>
static DecodeStatus decodeSImmOperand(MCInst &Inst, uint64_t Imm, uint64_t Address, const void *Decoder) {
  if (width == 8) {
    int8_t Imm8 = static_cast<int8_t>(Imm);
    Inst.addOperand(MCOperand::createImm(Imm8));
  } else if (width == 16) {
    int16_t Imm16 = static_cast<int16_t>(Imm);
    Inst.addOperand(MCOperand::createImm(Imm16));
  } else if (width == 32) {
    int32_t Imm32 = static_cast<int32_t>(Imm);
    Inst.addOperand(MCOperand::createImm(Imm32));
  } else if (width == 64) {
    int64_t Imm64 = static_cast<int64_t>(Imm);
    Inst.addOperand(MCOperand::createImm(Imm64));
  } else {
    return MCDisassembler::Fail;
  }
  return MCDisassembler::Success;
}

template <int width>
static DecodeStatus decodeImmOperand(MCInst &Inst, uint64_t Imm, uint64_t Address, const void *Decoder) {
  if (width == 8) {
    uint8_t Imm8 = static_cast<uint8_t>(Imm);
    Inst.addOperand(MCOperand::createImm(Imm8));
  } else if (width == 16) {
    uint16_t Imm16 = static_cast<uint16_t>(Imm);
    Inst.addOperand(MCOperand::createImm(Imm16));
  } else if (width == 32) {
    uint32_t Imm32 = static_cast<uint32_t>(Imm);
    Inst.addOperand(MCOperand::createImm(Imm32));
  } else if (width == 64) {
    uint64_t Imm64 = static_cast<uint64_t>(Imm);
    Inst.addOperand(MCOperand::createImm(Imm64));
  } else {
    return MCDisassembler::Fail;
  }
  return MCDisassembler::Success;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTriCoreDisassembler() {
  // Register the disassembler
  TargetRegistry::RegisterMCDisassembler(getTheTriCoreTarget(),
                                         createTriCoreDisassembler);
}

TriCoreDisassembler::TriCoreDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
    : MCDisassembler(STI, Ctx) {}

#include "TriCoreGenDisassemblerTables.inc"

DecodeStatus
TriCoreDisassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                  ArrayRef<uint8_t> Bytes, uint64_t Address,
                                  raw_ostream & /*CStream*/) const {

  MCDisassembler::DecodeStatus status = MCDisassembler::DecodeStatus::Fail; 
  Check(status, MCDisassembler::DecodeStatus::SoftFail);
  return status == MCDisassembler::DecodeStatus::Success ? status : MCDisassembler::DecodeStatus::Fail;
}

