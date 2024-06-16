//===-- TriCoreTargetMachine.cpp - Define TargetMachine for TriCore ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about TriCore target spec.
//
//===----------------------------------------------------------------------===//

#include "TriCoreTargetMachine.h"

#include "TriCore.h"
#include "TriCoreMachineFunctionInfo.h"
#include "TriCoreTargetObjectFile.h"
#include "TriCoreTargetTransformInfo.h"
#include "TargetInfo/TriCoreTargetInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetOptions.h"
#include <optional>

using namespace llvm;

namespace llvm {
void initializeTriCoreMemAluCombinerPass(PassRegistry &);
} // namespace llvm

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTriCoreTarget() {
  // Register the target.
  RegisterTargetMachine<TriCoreTargetMachine> registered_target(
      getTheTriCoreTarget());
  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeTriCoreDAGToDAGISelPass(PR);
}

static std::string computeDataLayout() {
  // Here you have to define the data layout for your target. Here is an example:
  // return "E"        // Big endian
  //        "-m:e"     // ELF name manging
  //        "-p:32:32" // 32-bit pointers, 32 bit aligned
  //        "-i64:64"  // 64 bit integers, 64 bit aligned
  //        "-a:0:32"  // 32 bit alignment of objects of aggregate type
  //        "-n32"     // 32 bit native integer width
  //        "-S64";    // 64 bit natural stack alignment
  // The Data layout has to be kept in sync with clang/lib/Basic/Targets.cpp
  return "e-m:e-p:32:32-i64:32-a:0:32-n32";
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::PIC_);
}

TriCoreTargetMachine::TriCoreTargetMachine(
    const Target &T, const Triple &TT, StringRef Cpu, StringRef FeatureString,
    const TargetOptions &Options, std::optional<Reloc::Model> RM,
    std::optional<CodeModel::Model> CodeModel, CodeGenOpt::Level OptLevel,
    bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(), TT, Cpu, FeatureString, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CodeModel, CodeModel::Medium),
                        OptLevel),
      Subtarget(TT, Cpu, FeatureString, *this, Options, getCodeModel(),
                OptLevel), TLOF(new TriCoreTargetObjectFile()), TLInfo(*this, Subtarget), FrameLowering(Subtarget) {
  initAsmInfo();
}

TargetTransformInfo
TriCoreTargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(TriCoreTTIImpl(this, F));
}

MachineFunctionInfo *TriCoreTargetMachine::createMachineFunctionInfo(
    BumpPtrAllocator &Allocator, const Function &F,
    const TargetSubtargetInfo *STI) const {
  return TriCoreMachineFunctionInfo::create<TriCoreMachineFunctionInfo>(Allocator,
                                                                    F, STI);
}

namespace {
// TriCore Code Generator Pass Configuration Options.
class TriCorePassConfig : public TargetPassConfig {
public:
  TriCorePassConfig(TriCoreTargetMachine &TM, PassManagerBase *PassManager)
      : TargetPassConfig(TM, *PassManager) {}

  TriCoreTargetMachine &getTriCoreTargetMachine() const {
    return getTM<TriCoreTargetMachine>();
  }

  bool addInstSelector() override;
  void addPreSched2() override;
  void addPreEmitPass() override;
};
} // namespace

TargetPassConfig *
TriCoreTargetMachine::createPassConfig(PassManagerBase &PassManager) {
  return new TriCorePassConfig(*this, &PassManager);
}

// Install an instruction selector pass.
bool TriCorePassConfig::addInstSelector() {
  addPass(createTriCoreISelDag(getTriCoreTargetMachine()));
  return false;
}

// Implemented by targets that want to run passes immediately before
// machine code is emitted.
void TriCorePassConfig::addPreEmitPass() {
}

// Run passes after prolog-epilog insertion and before the second instruction
// scheduling pass.
void TriCorePassConfig::addPreSched2() {
}
