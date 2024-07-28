//===-- TriCoreAsmBackend.cpp - TriCore Assembler Backend ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TriCoreMCTargetDesc.h"
#include "MCTargetDesc/TriCoreFixupKinds.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
class TriCoreAsmBackend : public MCAsmBackend {
  Triple::OSType OSType;
  MCFixupKindInfo FixupKindInfo;

public:
  TriCoreAsmBackend(const Target &T, Triple::OSType OST)
      : MCAsmBackend(support::big), OSType(OST) {}

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  // No instruction requires relaxation
  bool fixupNeedsRelaxation(const MCFixup & /*Fixup*/, uint64_t /*Value*/,
                            const MCRelaxableFragment * /*DF*/,
                            const MCAsmLayout & /*Layout*/) const override {
    return false;
  }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override;

  unsigned getNumFixupKinds() const override {
    return TRICORE::NumTargetFixupKinds;
  }

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
};

bool TriCoreAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                   const MCSubtargetInfo *STI) const {
  if (Count == 0) {
    return true;
  }
  return false;
}

static unsigned adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext *Ctx = NULL) {
  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case TRICORE::fixup_call:
    return Value & 0xffffff;
  case TRICORE::fixup_leg_mov_hi16_pcrel:
    Value >>= 16;
    // Intentional fall-through
  case TRICORE::fixup_leg_mov_lo16_pcrel:
    unsigned Hi4  = (Value & 0xF000) >> 12;
    unsigned Lo12 = Value & 0x0FFF;
    // inst{19-16} = Hi4;
    // inst{11-0} = Lo12;
    Value = (Hi4 << 16) | (Lo12);
    return Value;
  }
  return Value;
}

void TriCoreAsmBackend::applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                                 const MCValue &Target,
                                 MutableArrayRef<char> Data, uint64_t Value,
                                 bool IsResolved,
                                 const MCSubtargetInfo * /*STI*/) const 
{
  unsigned NumBytes = 4;
  Value = adjustFixupValue(Fixup, Value);
  if (!Value) {
    return; // Doesn't change encoding.
  }

  unsigned Offset = Fixup.getOffset();
  assert(Offset + NumBytes <= Data.size() && "Invalid fixup offset!");

  // For each byte of the fragment that the fixup touches, mask in the bits from
  // the fixup value. The Value has been "split up" into the appropriate
  // bitfields above.
  for (unsigned i = 0; i != NumBytes; ++i) {
    Data[Offset + i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

std::unique_ptr<MCObjectTargetWriter>
TriCoreAsmBackend::createObjectTargetWriter() const {
  return createTriCoreELFObjectWriter(MCELFObjectTargetWriter::getOSABI(OSType));
}

const MCFixupKindInfo &
TriCoreAsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[TRICORE::NumTargetFixupKinds] = {
      // This table *must* be in the order that the fixup_* kinds are defined in
      // TriCoreFixupKinds.h.
      //
      // { Name, Offset (bits), Size (bits), Flags }
      { "fixup_leg_mov_hi16_pcrel", 0, 32, MCFixupKindInfo::FKF_IsPCRel },
      { "fixup_leg_mov_lo16_pcrel", 0, 32, MCFixupKindInfo::FKF_IsPCRel },
      { "fixup_call", 0, 24, 0 },
    };

    if (Kind < FirstTargetFixupKind) {
      return MCAsmBackend::getFixupKindInfo(Kind);
    }

    assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
           "Invalid kind!");
    return Infos[Kind - FirstTargetFixupKind];
}

} // namespace

MCAsmBackend *llvm::createTriCoreAsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo & /*MRI*/,
                                          const MCTargetOptions & /*Options*/) {
  const Triple &TT = STI.getTargetTriple();
  if (!TT.isOSBinFormatELF())
    llvm_unreachable("OS not supported");

  return new TriCoreAsmBackend(T, TT.getOS());
}
