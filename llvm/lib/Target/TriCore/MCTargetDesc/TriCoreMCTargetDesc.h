//===-- TriCoreMCTargetDesc.h - TriCore Target Descriptions ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides TriCore specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_MCTARGETDESC_TRICOREMCTARGETDESC_H
#define LLVM_LIB_TARGET_TRICORE_MCTARGETDESC_TRICOREMCTARGETDESC_H

#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCSubtargetInfo;
class Target;

// extern Target TheTriCoreTarget;

MCAsmBackend *createTriCoreAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createTriCoreELFObjectWriter(uint8_t OSABI);

MCCodeEmitter *createTriCoreMCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx);

} // namespace llvm

// Defines symbolic names for TriCore registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "TriCoreGenRegisterInfo.inc"

// Defines symbolic names for the TriCore instructions.
#define GET_INSTRINFO_ENUM
#include "TriCoreGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "TriCoreGenSubtargetInfo.inc"

#endif // LLVM_LIB_TARGET_TRICORE_MCTARGETDESC_TRICOREMCTARGETDESC_H
