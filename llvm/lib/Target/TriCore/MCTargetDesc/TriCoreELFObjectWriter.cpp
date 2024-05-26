//===-- TriCoreELFObjectWriter.cpp - TriCore ELF Writer -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {

class TriCoreELFObjectWriter : public MCELFObjectTargetWriter {
public:
  explicit TriCoreELFObjectWriter(uint8_t OSABI);

  ~TriCoreELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
  bool needsRelocateWithSymbol(const MCSymbol &SD,
                               unsigned Type) const override;
};

} // end anonymous namespace

TriCoreELFObjectWriter::TriCoreELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit_=*/false, OSABI, ELF::EM_TRICORE,
                              /*HasRelocationAddend_=*/true) {}

unsigned TriCoreELFObjectWriter::getRelocType(MCContext & /*Ctx*/,
                                            const MCValue & /*Target*/,
                                            const MCFixup &Fixup,
                                            bool /*IsPCRel*/) const {
  return 0;
}

bool TriCoreELFObjectWriter::needsRelocateWithSymbol(const MCSymbol & /*SD*/,
                                                   unsigned Type) const {
  return false;
}

std::unique_ptr<MCObjectTargetWriter> createTriCoreELFObjectWriter(uint8_t OSABI)
{
  return std::make_unique<TriCoreELFObjectWriter>(OSABI);
}
