//===- TriCore.cpp ---------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Symbols.h"
#include "Target.h"
#include "lld/Common/ErrorHandler.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {
class TriCore final : public TargetInfo {
public:
  TriCore();
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  void relocate(uint8_t *loc, const Relocation &rel,
                uint64_t val) const override;
};
} // namespace

TriCore::TriCore() {
  // mov.b #0, r3
  trapInstr = {0x43, 0x43, 0x43, 0x43};
}

RelExpr TriCore::getRelExpr(RelType type, const Symbol &s,
                           const uint8_t *loc) const {
  switch (type) {
  case R_TRICORE_PCPHI:
  case R_TRICORE_PCPLO:
  case R_TRICORE_PCPPAGE:
  case R_TRICORE_PCPOFF:
  case R_TRICORE_PCPTEXT:
    return R_PC;
  default:
    return R_ABS;
  }
}

void TriCore::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {}

TargetInfo *elf::getTriCoreTargetInfo() {
  static TriCore target;
  return &target;
}
