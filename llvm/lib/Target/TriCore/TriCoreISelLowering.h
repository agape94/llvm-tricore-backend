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
  enum NodeType : unsigned {
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
private:
  const TargetMachine &TM;
  const TriCoreSubtarget &Subtarget;
public:

  /// LowerOperation - Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  TriCoreTargetLowering(const TargetMachine &TM, const TriCoreSubtarget &STI);
  
  /// getTargetNodeName - This method returns the name of a target specific
  //  DAG node.
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

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                      SmallVectorImpl<SDValue> &InVals) const override;


  SDValue LowerCallResult(SDValue Chain, SDValue InGlue,
                          CallingConv::ID CallConv, bool isVarArg,
                          const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc dl,
                          SelectionDAG &DAG,
                          SmallVectorImpl<SDValue> &InVals) const;

  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &ArgsFlags,
                      LLVMContext &Context) const override;

  // LowerGlobalAddress - Emit a constant load to the global address.
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

  MachineBasicBlock* EmitInstrWithCustomInserter(MachineInstr &MI,
                                                  MachineBasicBlock *BB) const override;

  // Lower Branch
  SDValue LowerBR_CC(SDValue Op, SelectionDAG &DAG) const;

  // Lower SELECT_CC
  SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerSETCC(SDValue Op, SelectionDAG &DAG) const;

  // Lower Shift Instruction
  SDValue LowerShifts(SDValue Op, SelectionDAG &DAG) const;

  // Lower Mul High Instruction
  SDValue LowerMulHigh(SDValue Op, SelectionDAG &DAG) const;

  const TriCoreRegisterInfo *TRI;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICOREISELLOWERING_H
