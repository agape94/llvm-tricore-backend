//===-- TriCoreTargetMachine.h - Define TargetMachine for TriCore --- C++ ---===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the TriCore specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICORETARGETMACHINE_H
#define LLVM_LIB_TARGET_TRICORE_TRICORETARGETMACHINE_H

#include "TriCoreISelLowering.h"
#include "TriCoreInstrInfo.h"
#include "TriCoreSelectionDAGInfo.h"
#include "TriCoreSubtarget.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {

class TriCoreTargetMachine : public LLVMTargetMachine {
  TriCoreSubtarget Subtarget;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  
  TriCoreInstrInfo InstrInfo;
  TriCoreTargetLowering TLInfo;
  TriCoreFrameLowering FrameLowering;


public:
  TriCoreTargetMachine(const Target &TheTarget, const Triple &TargetTriple,
                     StringRef Cpu, StringRef FeatureString,
                     const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CodeModel,
                     CodeGenOpt::Level OptLevel, bool JIT);

  const TriCoreSubtarget *
  getSubtargetImpl(const llvm::Function & /*Fn*/) const override {
    return &Subtarget;
  }

  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &pass_manager) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;

  bool isMachineVerifierClean() const override {
    return false;
  }

  virtual const TargetRegisterInfo *getRegisterInfo() const 
  {
    return &InstrInfo.getRegisterInfo();
  }
  
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICORETARGETMACHINE_H
