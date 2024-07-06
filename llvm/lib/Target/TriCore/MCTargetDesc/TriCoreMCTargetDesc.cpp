//===-- TriCoreMCTargetDesc.cpp - TriCore Target Descriptions -----------------===//
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

#include "TriCoreMCTargetDesc.h"
#include "TriCoreInstPrinter.h"
#include "TriCoreMCAsmInfo.h"
#include "TargetInfo/TriCoreTargetInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/TargetParser/Triple.h"
#include <cstdint>
#include <string>

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "TriCoreGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "TriCoreGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "TriCoreGenRegisterInfo.inc"

using namespace llvm;

static MCInstPrinter *createTriCoreMCInstPrinter(const Triple & /*T*/,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  return new TriCoreInstPrinter(MAI, MII, MRI);
}

static MCSubtargetInfo *
createTriCoreMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  std::string CPUName = std::string(CPU);
  if (CPUName.empty())
    CPUName = "generic";

  return createTriCoreMCSubtargetInfoImpl(TT, CPUName, /*TuneCPU*/ CPUName, FS);
}

static MCInstrInfo *createTriCoreMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitTriCoreMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createTriCoreMCRegisterInfo(const Triple & /*TT*/) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitTriCoreMCRegisterInfo(X, TRICORE::D0);
  return X;
}

static MCAsmInfo *createTriCoreMCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  return new TriCoreMCAsmInfo(TT, Options);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTriCoreTargetMC() {
  Target &T = getTheTriCoreTarget();
  
  // Register the MC asm info.
  TargetRegistry::RegisterMCAsmInfo(T, createTriCoreMCAsmInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(T, createTriCoreMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(T, createTriCoreMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(T,
                                          createTriCoreMCSubtargetInfo);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(T, createTriCoreMCInstPrinter);

  // Register the ASM Backend.
  TargetRegistry::RegisterMCAsmBackend(T, createTriCoreAsmBackend);

  // Register the MCCodeEmitter
  TargetRegistry::RegisterMCCodeEmitter(T, createTriCoreMCCodeEmitter);
}
