//===-- TriCoreISelLowering.h - TriCore DAG Lowering Interface -....-*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that TriCore uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICOREISELLOWERING_H
#define LLVM_LIB_TARGET_TRICORE_TRICOREISELLOWERING_H

#include "TriCore.h"
#include "TriCoreRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {
namespace TRICOREISD {
enum {
// Start the numbering where the builtin ops and target ops leave off.
      FIRST_NUMBER = ISD::BUILTIN_OP_END,
      RET_FLAG,
      // This loads the symbol (e.g. global address) into a register.
      LOAD_SYM,
      // This loads a 32-bit immediate into a register.
      MOVEi32,
      CALL,
      // TriCore has a different way of lowering branch conditions.
      BR_CC,
      // This loads the comparison type, as Tricore doesn't support all
      // sorts of comparisons, some have to be created.
      CMP,
      // This load the addressing information
      Wrapper,
      // This loads the Shift instructions operands. Right and left shift
      // depends on the signed-ness on the shift value. A negytive value is
      // a right shift, and vice versa.
      SH,
      // Arithmetic Shift
      SHA,
      // Loads ternary operators
      SELECT_CC,
      LOGICCMP,
      IMASK,
      EXTR,
      ABS
};
} // namespace TRICOREISD

class TriCoreSubtarget;

class TriCoreTargetLowering : public TargetLowering {
public:
  TriCoreTargetLowering(const TargetMachine &TM, const TriCoreSubtarget &STI);
  
  const char *getTargetNodeName(unsigned Opcode) const override;
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                              bool IsVarArg,
                              const SmallVectorImpl<ISD::InputArg> &Ins,
                              const SDLoc &DL, SelectionDAG &DAG,
                              SmallVectorImpl<SDValue> &InVals) const override;
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                    const SmallVectorImpl<ISD::OutputArg> &Outs,
                    const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                    SelectionDAG &DAG) const override;
  const TriCoreRegisterInfo *TRI;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICOREISELLOWERING_H
