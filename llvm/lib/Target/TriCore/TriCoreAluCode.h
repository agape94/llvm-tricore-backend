//===-- TriCoreAluCode.h - ALU operator encoding ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The encoding for ALU operators used in RM and RRM operands
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICOREALUCODE_H
#define LLVM_LIB_TARGET_TRICORE_TRICOREALUCODE_H

#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {
namespace LPAC {
enum AluCode {
  // Indicates an unknown/unsupported operator
  UNKNOWN = 0xFF,
};

inline static unsigned getAluOp(unsigned AluOp) {
  unsigned const ALU_MASK = 0x3F;
  return AluOp & ALU_MASK;
}

inline static const char *tricoreAluCodeToString(unsigned AluOp) {
  switch (getAluOp(AluOp)) {
  default:
    llvm_unreachable("Invalid ALU code.");
  }
}

inline static AluCode stringToTriCoreAluCode(StringRef S) {
  return StringSwitch<AluCode>(S)
      .Default(UNKNOWN);
}

inline static AluCode isdToTriCoreAluCode(ISD::NodeType Node_type) {
  switch (Node_type) {
  default:
    return AluCode::UNKNOWN;
  }
}
} // namespace LPAC
} // namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICOREALUCODE_H
