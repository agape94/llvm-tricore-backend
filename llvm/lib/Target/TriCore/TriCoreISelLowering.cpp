//===-- TriCoreISelLowering.cpp - TriCore DAG Lowering Implementation ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the TriCoreTargetLowering class.
//
//===----------------------------------------------------------------------===//

#include "TriCoreISelLowering.h"
#include "TriCore.h"
#include "TriCoreMachineFunctionInfo.h"
#include "TriCoreSubtarget.h"
#include "TriCoreTargetObjectFile.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineValueType.h"
#include "llvm/CodeGen/RuntimeLibcalls.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/TargetCallingConv.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/KnownBits.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <iostream>

#define DEBUG_TYPE "tricore-lower"

using namespace llvm;

#include "TriCoreGenCallingConv.inc"

SDValue TriCoreTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const 
{
  return Chain;
}

SDValue TriCoreTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs,
    const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
    SelectionDAG &DAG) const  
{
  if (IsVarArg) {
    report_fatal_error("VarArg not supported");
  }

  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;

  Type* t= DAG.getMachineFunction().getFunction().getReturnType();

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_TriCore);

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    CCValAssign &VA = RVLocs[i];

    // if it is a pointer, we have to store in A2
    if (t->isPointerTy())
      VA.convertToReg(TRICORE::A2);

    assert(VA.isRegLoc() && "Can only return in registers!");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);

    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

    RetOps[0] = Chain; // Update chain.

    // Add the flag if we have it.
    if (Flag.getNode()) {
      RetOps.push_back(Flag);
    }

    return DAG.getNode(TRICOREISD::RET_FLAG, DL, MVT::Other, RetOps);
}

const char *TriCoreTargetLowering::getTargetNodeName(unsigned Opcode) const
{
switch ((TRICOREISD::NodeType)Opcode) {
    case TRICOREISD::FIRST_NUMBER:  break;
    case TRICOREISD::RET_FLAG:      return "TRICOREISD::RetFlag";
    case TRICOREISD::LOAD_SYM:      return "TRICOREISD::LOAD_SYM";
    case TRICOREISD::MOVEi32:       return "TRICOREISD::MOVEi32";
    case TRICOREISD::CALL:          return "TRICOREISD::CALL";
    case TRICOREISD::BR_CC:         return "TRICOREISD::BR_CC";
    case TRICOREISD::SELECT_CC:     return "TRICOREISD::SELECT_CC";
    case TRICOREISD::LOGICCMP:      return "TRICOREISD::LOGICCMP";
    case TRICOREISD::CMP:           return "TRICOREISD::CMP";
    case TRICOREISD::IMASK:         return "TRICOREISD::IMASK";
    case TRICOREISD::Wrapper:       return "TRICOREISD::Wrapper";
    case TRICOREISD::SH:            return "TRICOREISD::SH";
    case TRICOREISD::SHA:           return "TRICOREISD::SHA";
    case TRICOREISD::EXTR:          return "TRICOREISD::EXTR";      
    case TRICOREISD::ABS:           return "TRICOREISD::ABS";
  }
  return nullptr;
}

TriCoreTargetLowering::TriCoreTargetLowering(const TargetMachine &TM,
                                            const TriCoreSubtarget &STI)
    : TargetLowering(TM), TM(TM), Subtarget(STI) 
{
  // Set up the register classes.
  addRegisterClass(MVT::i32, &TRICORE::DataRegsRegClass);
  //addRegisterClass(MVT::i32, &TRICORE::AddrRegsRegClass);
  addRegisterClass(MVT::i64, &TRICORE::DataExtRegsRegClass);
  //addRegisterClass(MVT::i32, &TRICORE::PSRegsRegClass);


  // Compute derived properties from the register classes
  // computeRegisterProperties(Subtarget.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(TRICORE::A10);

  // Nodes that require custom lowering
  setOperationAction(ISD::GlobalAddress, MVT::i32,   Custom);
  setOperationAction(ISD::BR_CC,         MVT::i32,   Custom);
  setOperationAction(ISD::BR_CC,         MVT::i64,   Custom);
  setOperationAction(ISD::SELECT_CC,     MVT::i32,   Custom);
  setOperationAction(ISD::SETCC,         MVT::i32,   Custom);
  setOperationAction(ISD::SHL,           MVT::i32,   Custom);
  setOperationAction(ISD::SRL,           MVT::i32,   Custom);
  setOperationAction(ISD::SRA,           MVT::i32,   Custom);
  //setOperationAction(ISD::SRA,           MVT::i16,   Custom);
  //setOperationAction(ISD::SIGN_EXTEND,   MVT::i16,   Expand);

  //for (MVT VT : MVT::integer_valuetypes())
  //setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16,   Custom);

  // Compute derived properties from the register classes
  TRI = STI.getRegisterInfo();
  computeRegisterProperties(TRI);

  setSchedulingPreference(Sched::Source);

  // Booleans always contain 0 or 1.
  setBooleanContents(ZeroOrOneBooleanContent);
}
