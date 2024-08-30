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

#define UNKNOWN_REG 5555

using namespace llvm;

struct regInfo{
  StringRef fName;
  unsigned reg;
  bool isPointer;
};

class TriCoreCallingConvHook {


private:
  uint32_t curPos = 0;
  uint32_t curArg = 0;


  std::vector< regInfo > regRecord;

public:
  TriCoreCallingConvHook() : curPos(0), curArg(0) {}

  bool isRegValPtrType (MachineFunction& _mf)
  {
    Function::const_arg_iterator FI;
    FI = _mf.getFunction().arg_begin();
    std::advance(FI,curArg);
    return FI->getType()->isPointerTy();
  }

  bool isRegValid64Type (MachineFunction& _mf)
  {
    Function::const_arg_iterator FI;
    FI = _mf.getFunction().arg_begin();
    std::advance(FI,curArg);
    outs() << "size: " << FI->getType()->getScalarSizeInBits() << "\n";
    return (FI->getType()->getScalarSizeInBits() == 64);
  }

  void init()
  {
    curPos = 0;
    curArg = 0;
    regRecord.clear();
  }
  
  void setCurPos(uint32_t curPos = 0) {this->curPos = curPos;}
  void setArgPos(uint32_t curArg = 0) {this->curArg = curArg;}

  void incrArgPos() {this->curArg++;}

  TriCoreCallingConvHook operator++(int)
  {
    this->curPos++;
    return *this;
  }

  uint32_t operator()() { return this->curPos;}

  int32_t findInRegRecord(StringRef funString)
  {
    uint32_t index = 0;
    for (auto& rec : regRecord) {
      if(rec.fName.compare(funString) == 0){
        return index;
      }
      index++;
    }
    return -1;
  }

  int32_t findInRegLastRecord(StringRef funString)
  {
    uint32_t index = regRecord.size();
    for (int32_t i = index-1; i>=0; i--) {
      if(regRecord[i].fName.compare(funString) == 0)
        return i;
    }
    return -1;
  }

  uint32_t getCurPos() const {  return this->curPos;}
  uint32_t getArgPos() const {  return curArg;}

  unsigned getNextAddrRegs(StringRef fName)
  {
    std::vector<unsigned> regList;
    for (auto& rec : regRecord) {
      if (rec.fName==fName && rec.reg >= TRICORE::A4 && rec.reg <= TRICORE::A7)
        regList.push_back(rec.reg);
    }
    if (regList.size() == 0)
      return TRICORE::A4;

    unsigned lastReg = regList.back();

    if (lastReg == TRICORE::A7)
      return UNKNOWN_REG;

    return lastReg + 1;
  }

  unsigned getNextDataRegs(StringRef fName)
  {
    std::vector<unsigned> regList;
    for (auto& rec : regRecord) {
      if (rec.fName==fName && rec.reg >= TRICORE::D4 && rec.reg <= TRICORE::D7)
        regList.push_back(rec.reg);
    }
    if (regList.size() == 0)
      return TRICORE::D4;

    unsigned lastReg = regList.back();

    if (lastReg == TRICORE::D7)
      return UNKNOWN_REG;
    return lastReg + 1;
  }

  unsigned getNextExtRegs(StringRef fName)
  {
    std::vector<unsigned> regList;
    for (auto& rec : regRecord) {
      if (rec.fName==fName && rec.reg >= TRICORE::E4 && rec.reg <= TRICORE::E6)
        regList.push_back(rec.reg);
    }
    if (regList.size() == 0)
      return TRICORE::E4;

    unsigned lastReg = regList.back();

    if (lastReg == TRICORE::E6)
      return UNKNOWN_REG;
    return lastReg + 1;
  }

  bool getRegRecordisPointer(uint32_t pos) {return regRecord[pos].isPointer;}
  StringRef getFunctionName(uint32_t pos) {return regRecord[pos].fName;}
  unsigned getRegRecordRegister(uint32_t pos) {return regRecord[pos].reg;}
  uint32_t getNumOfArgs(StringRef fName)
  {
    uint32_t count = 0;
    for (auto& rec : regRecord) {
      if(rec.fName.compare(fName)==0)
        count++;
    }
    return count;
  }

  void saveRegRecord(StringRef funName, unsigned reg, bool isPointer)
  {
    struct regInfo tmp;
    tmp.fName = funName;
    tmp.reg = reg;
    tmp.isPointer = isPointer;
    regRecord.push_back(tmp);
  }

  void printRegRecord()
  {
    for (auto& rec : regRecord)
    outs()<< rec.fName <<"\t"<<rec.reg<<"\t"<<rec.isPointer<<"\n";
  }
};

TriCoreCallingConvHook TCCH;

SDValue TriCoreTargetLowering::
LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {  
    case ISD::GlobalAddress:      return LowerGlobalAddress(Op, DAG);
    case ISD::BR_CC:              return LowerBR_CC(Op, DAG);
    case ISD::SELECT_CC:          return LowerSELECT_CC(Op, DAG);
    case ISD::SETCC:              return LowerSETCC(Op, DAG);
    case ISD::SHL:
    case ISD::SRL:
    case ISD::SRA:                return LowerShifts(Op, DAG);

    case ISD::MULHS:
    case ISD::MULHU:
      return LowerMulHigh(Op, DAG);
    //case ISD::SIGN_EXTEND:        return LowerSIGN_EXTEND(Op, DAG);
    //case ISD::SIGN_EXTEND_INREG:  return LowerSIGN_EXTEND_INREG(Op, DAG);
    default:
      llvm_unreachable("unimplemented operand");
  }
}

SDValue TriCoreTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const 
{
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();

  assert(!IsVarArg && "VarArg not supported");

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;

  //get incoming arguments information
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
      *DAG.getContext());

  StringRef funName = DAG.getMachineFunction().getFunction().getName();
  std::cout << "Function Name: " << funName.str() << "\n";

//  DAG.getMachineFunction().getFunction()

  CCInfo.AnalyzeFormalArguments(Ins, CC_TriCore);
  

  TCCH.init();

  for (uint32_t i = 0; i < ArgLocs.size(); i++) {

    CCValAssign VA(ArgLocs[i]);

    SDValue ArgIn;
    unsigned AddrReg;
    if (TCCH.isRegValPtrType(MF)) {
      outs() << "XXXXXXXXX Pointer Type\n";
      //Is there any address register available?
      AddrReg = TCCH.getNextAddrRegs(funName);
      if (AddrReg != UNKNOWN_REG)
        VA.convertToReg(AddrReg);
      }
    else if (TCCH.isRegValid64Type(MF)) {
      unsigned ExtReg = TCCH.getNextExtRegs(funName);
      if (ExtReg != UNKNOWN_REG)
        VA.convertToReg(ExtReg);
    }
    else {
      unsigned DataReg = TCCH.getNextDataRegs(funName);
      if (DataReg != UNKNOWN_REG)
        VA.convertToReg(DataReg);
    }

    if (VA.isRegLoc()) {
      // Arguments passed in registers
      EVT RegVT = VA.getLocVT();
      assert( (RegVT.getSimpleVT().SimpleTy == MVT::i32 ||
             RegVT.getSimpleVT().SimpleTy == MVT::i64)
              && "supports MVT::i32 and  MVT::i64  register passing");

      unsigned VReg;

      // If the argument is a pointer type then create a AddrRegsClass
      // Virtual register.
      if (TCCH.isRegValPtrType(MF)) {
        VA.setValVT(MVT::iPTR);
        VReg = RegInfo.createVirtualRegister(&TRICORE::AddrRegsRegClass);
        RegInfo.addLiveIn(VA.getLocReg(), VReg); //mark the register is inuse
        TCCH.saveRegRecord(funName, VA.getLocReg(), true);
        TCCH++;
        ArgIn = DAG.getCopyFromReg(Chain, DL, VReg, RegVT, MVT::iPTR);
      }
      else if (TCCH.isRegValid64Type(MF))  {
        VReg = RegInfo.createVirtualRegister(&TRICORE::DataExtRegsRegClass);
        RegInfo.addLiveIn(VA.getLocReg(), VReg); //mark the register is inuse
        TCCH.saveRegRecord(funName, VA.getLocReg(), false);
        ArgIn = DAG.getCopyFromReg(Chain, DL, VReg, MVT::i64);
        TCCH++;
      }
      // else place it inside a data register.
      else {
        VReg = RegInfo.createVirtualRegister(&TRICORE::DataRegsRegClass);
        RegInfo.addLiveIn(VA.getLocReg(), VReg); //mark the register is inuse
        TCCH.saveRegRecord(funName, VA.getLocReg(), false);
        ArgIn = DAG.getCopyFromReg(Chain, DL, VReg, MVT::i32);
        TCCH++;
      }


      InVals.push_back(ArgIn);
      TCCH.incrArgPos();
      continue;
    }

    assert(
        VA.isMemLoc()
            && "Can only pass arguments as either registers or via the stack");

    const unsigned Offset = VA.getLocMemOffset();

    // create stack offset it the input argument is placed in memory

    uint64_t size = 4;
    if (VA.getValVT() == MVT::i64)
      size = 8;

    const int FI = MF.getFrameInfo().CreateFixedObject(size, Offset, true);
    EVT PtrTy = getPointerTy(DAG.getDataLayout());
    SDValue FIPtr = DAG.getFrameIndex(FI, PtrTy);

    SDValue Load = DAG.getLoad(VA.getValVT(), DL, Chain, FIPtr, MachinePointerInfo());

    InVals.push_back(Load);
    TCCH.incrArgPos();
  }

  TCCH.setCurPos(0);
  //TCCH.printRegRecord();

  return Chain;
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

  // Compute derived properties from the register classes
  TRI = STI.getRegisterInfo();
  computeRegisterProperties(TRI);

  setSchedulingPreference(Sched::Source);

  // Nodes that require custom lowering
  setOperationAction(ISD::GlobalAddress, MVT::i32,   Custom);
  setOperationAction(ISD::BR_CC,         MVT::i32,   Custom);
  setOperationAction(ISD::BR_CC,         MVT::i64,   Custom);
  setOperationAction(ISD::SELECT_CC,     MVT::i32,   Custom);
  setOperationAction(ISD::SETCC,         MVT::i32,   Custom);
  setOperationAction(ISD::SHL,           MVT::i32,   Custom);
  setOperationAction(ISD::SRL,           MVT::i32,   Custom);
  setOperationAction(ISD::SRA,           MVT::i32,   Custom);

  setOperationAction({ISD::MUL, ISD::SDIV, ISD::UDIV}, MVT::i8, Promote);
  setOperationAction({ISD::MUL, ISD::SDIV, ISD::UDIV}, MVT::i16, Promote);
  setOperationAction({ISD::MUL, ISD::SDIV, ISD::UDIV}, MVT::i32, Legal);
  // setOperationAction(ISD::MUL, MVT::i64, LibCall);

  for (auto OP :
       {ISD::SREM, ISD::UREM, ISD::UDIVREM, ISD::SDIVREM,
        ISD::MULHS, ISD::MULHU, ISD::UMUL_LOHI, ISD::SMUL_LOHI}) {
    setOperationAction(OP, MVT::i8, Custom);
    setOperationAction(OP, MVT::i16, Custom);
    setOperationAction(OP, MVT::i32, Custom);
  }

  for (auto OP : {ISD::UMUL_LOHI, ISD::SMUL_LOHI}) {
    setOperationAction(OP, MVT::i8, Expand);
    setOperationAction(OP, MVT::i16, Expand);
  }
  //setOperationAction(ISD::SRA,           MVT::i16,   Custom);
  //setOperationAction(ISD::SIGN_EXTEND,   MVT::i16,   Expand);

  //for (MVT VT : MVT::integer_valuetypes())
  //setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16,   Custom);


  // Booleans always contain 0 or 1.
  // setBooleanContents(ZeroOrOneBooleanContent);
}

// TriCore call implementation
SDValue TriCoreTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                     SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &Loc = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  CallingConv::ID CallConv = CLI.CallConv;
  const bool isVarArg = CLI.IsVarArg;


  CLI.IsTailCall = false;

  if (isVarArg) {
    llvm_unreachable("Unimplemented. Variable number of arguments not supported!");
  }

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_TriCore);

    // Get the size of the outgoing arguments stack space requirement.
  // const unsigned NumBytes = CCInfo.getStackSize();
  const unsigned NumBytes = CCInfo.getAlignedCallFrameSize();

  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, NumBytes, Loc);
  // Chain = DAG.getCALLSEQ_START(Chain, DAG.getIntPtrConstant(NumBytes, Loc, true), Loc);

  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;

  // We only support calling global addresses.
  GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee);
  assert(G && "We only support the calling of global addresses");
  Callee = DAG.getTargetGlobalAddress(G->getGlobal(), Loc, MVT::i32);

  int32_t originalArgPos = TCCH.findInRegRecord(G->getGlobal()->getName());
  uint32_t argNum = TCCH.getNumOfArgs(G->getGlobal()->getName());
  TCCH.init();
  TCCH.setArgPos(originalArgPos);
  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    if(i<argNum)
      VA.convertToReg(TCCH.getRegRecordRegister(TCCH.getArgPos()));
    SDValue Arg = OutVals[i];

    // We only handle fully promoted arguments.
    assert(VA.getLocInfo() == CCValAssign::Full && "Unhandled loc info");

    if (VA.isRegLoc()) {
      RegsToPass.push_back(
              std::make_pair(VA.getLocReg(), Arg));
      TCCH.incrArgPos();
      continue;
    }
    assert(VA.isMemLoc() &&
           "Only support passing arguments through registers or via the stack");

    SDValue StackPtr = DAG.getRegister(TRICORE::A10, MVT::i32);
    SDValue PtrOff = DAG.getIntPtrConstant(VA.getLocMemOffset(), Loc);
    PtrOff = DAG.getNode(ISD::ADD, Loc, MVT::i32, StackPtr, PtrOff);
    MemOpChains.push_back(DAG.getStore(Chain, Loc, Arg, PtrOff, MachinePointerInfo()));
  }

  // Emit all stores, make sure they occur before the call.
  if (!MemOpChains.empty()) {
    Chain = DAG.getNode(ISD::TokenFactor, Loc, MVT::Other, MemOpChains);
  }

  // Build a sequence of copy-to-reg nodes chained together with token chain
  // and flag operands which copy the outgoing args into the appropriate regs.
  SDValue InFlag;
  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, Loc, Reg.first, Reg.second, InFlag);
    InFlag = Chain.getValue(1);
  }

  std::vector<SDValue> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are known live
  // into the call.
  for (auto &Reg : RegsToPass) {
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));
  }

  // Add a register mask operand representing the call-preserved registers.
  // const uint32_t *Mask;
  // const TargetRegisterInfo *TRI = DAG.getSubtarget().getRegisterInfo();
  // Mask = TRI->getCallPreservedMask(DAG.getMachineFunction(), CallConv);

  // assert(Mask && "Missing call preserved mask for calling convention");
  // Ops.push_back(DAG.getRegisterMask(Mask));

  if (InFlag.getNode()) {
    Ops.push_back(InFlag);
  }

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  // Returns a chain and a flag for retval copy to use.
  Chain = DAG.getNode(TRICOREISD::CALL, Loc, NodeTys, Ops);
  InFlag = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, DAG.getIntPtrConstant(NumBytes, Loc, true),
                             DAG.getIntPtrConstant(0, Loc, true), InFlag, Loc);
  if (!Ins.empty()) {
    InFlag = Chain.getValue(1);
  }


  //TCCH.setArgPos(originalArgPos);
  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, isVarArg, Ins, Loc, DAG,
                         InVals);
}

SDValue TriCoreTargetLowering::LowerCallResult(
    SDValue Chain, SDValue InGlue, CallingConv::ID CallConv, bool isVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, SDLoc dl, SelectionDAG &DAG,
    SmallVectorImpl<SDValue> &InVals) const {
  assert(!isVarArg && "Unsupported");
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  Type* t= DAG.getMachineFunction().getFunction().getReturnType();
  // t->dump();

  CCInfo.AnalyzeCallResult(Ins, RetCC_TriCore);
  //DAG.getMachineFunction().getFunction()->get
  // Copy all of the result registers out of their specified physreg.
  for (auto &Loc : RVLocs) {

    if (t->isPointerTy())
    {
      Loc.convertToReg(TRICORE::A2);
    }

    Chain = DAG.getCopyFromReg(Chain, dl, Loc.getLocReg(), Loc.getValVT(),
                               InGlue).getValue(1);
    InGlue = Chain.getValue(2);
    InVals.push_back(Chain.getValue(0));
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

bool TriCoreTargetLowering::CanLowerReturn(
    CallingConv::ID CallConv, MachineFunction &MF, bool isVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, isVarArg, MF, RVLocs, Context);
  if (!CCInfo.CheckReturn(Outs, RetCC_TriCore)) {
    return false;
  }
  if (CCInfo.getStackSize() != 0 && isVarArg) {
    return false;
  }
  return true;
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

SDValue TriCoreTargetLowering::LowerMulHigh(SDValue Op,
                                           SelectionDAG &DAG) const {
  // unsigned Opc = Op.getOpcode();
  // SDNode* N = Op.getNode();
  // SDValue LHS = N->getOperand(0);
  // SDValue RHS = N->getOperand(1);
  // // Promote both operands to i32
  // if (LHS.getValueType() != MVT::i32 && LHS.getValueType() != MVT::i64) {
  //   LHS = DAG.getNode(ISD::SIGN_EXTEND, SDLoc(N), MVT::i32, LHS);
  // }
  
  // if (RHS.getValueType() != MVT::i32 && RHS.getValueType() != MVT::i64) {
  //   RHS = DAG.getNode(ISD::SIGN_EXTEND, SDLoc(N), MVT::i32, RHS);
  // }

  // // Check if signed or unsigned
  // bool isSigned = Opc == ISD::MULHS;
  
  // // Place both operands in a register if not yet a register
  // if (LHS.getOpcode() != ISD::CopyToReg) {
  //   LHS = DAG.getCopyToReg(DAG.getEntryNode(), SDLoc(N), LHS, UNKNOWN_REG);
  // }
  // if (RHS != ISD::CopyToReg) {
  //   RHS = DAG.getCopyToReg(DAG.getEntryNode(), SDLoc(N), RHS, UNKNOWN_REG);
  // }

  // SDLoc dl(N);
  // SDValue Result = DAG.getNode(Opc, dl, MVT::i32, LHS, RHS);
  EVT VT = Op.getValueType();
  SDLoc DL(Op);

  // Perform the multiplication and get the high part.
  // This is a simplified example; the actual code will depend on the TriCore architecture.
  SDValue Res;
  Res = DAG.getNode(ISD::MUL, DL, VT, Op.getOperand(0), Op.getOperand(1));

  // The high part is the result shifted right by the size of the low part.
  SDValue ResHi = DAG.getNode(ISD::SRL, DL, VT, Res, DAG.getConstant(VT.getSizeInBits(), DL, VT));

  return ResHi;
}

SDValue TriCoreTargetLowering::LowerShifts(SDValue Op,
                                           SelectionDAG &DAG) const {
  unsigned Opc = Op.getOpcode();
  SDNode* N = Op.getNode();
  SDValue shiftValue =  N->getOperand(1);

  EVT VT = Op.getValueType();
  SDLoc dl(N);
  outs() << "Opc: " << Opc << "\n";
  switch (Opc) {
  default: llvm_unreachable("Invalid shift opcode!");
  case ISD::SHL:
    return DAG.getNode(TRICOREISD::SH, dl, VT, N->getOperand(0), N->getOperand(1));
  case ISD::SRL:
  case ISD::SRA:
    if(isa<ConstantSDNode>(shiftValue)) {
      outs() <<"shift constant\n";
      int64_t shiftSVal = cast<ConstantSDNode>(shiftValue)->getSExtValue();
      assert((shiftSVal>=-32 && shiftSVal<32) &&
              "Shift can only be between -32 and +31");
      ConstantSDNode *shiftSD = cast<ConstantSDNode>(N->getOperand(1));
      uint64_t shiftVal = -shiftSD->getZExtValue();
      SDValue negShift = DAG.getConstant(shiftVal, dl, MVT::i32);

      unsigned Opcode = (Opc== ISD::SRL) ? TRICOREISD::SH : TRICOREISD::SHA;

      return DAG.getNode(Opcode, dl, VT, N->getOperand(0), negShift);
    }
    else { // shift value is stored in a register

      SDValue regOP = N->getOperand(1);
      // Create subtraction node
      SDValue zero = DAG.getConstant(0, dl, MVT::i32);
      SDValue rsubNode = DAG.getNode(ISD::SUB, dl, MVT::i32, zero, regOP);
      unsigned Opcode = (Opc== ISD::SRL) ? TRICOREISD::SH : TRICOREISD::SHA;
      return DAG.getNode(Opcode, dl, MVT::i32, N->getOperand(0), rsubNode);
    }
  }
}

static SDValue EmitCMP(SDValue &LHS, SDValue &RHS, ISD::CondCode CC,
                       SDLoc dl, SelectionDAG &DAG, SDValue &tricoreCC) {
  // FIXME: Handle bittests someday
  assert(!LHS.getValueType().isFloatingPoint() && "We don't handle FP yet");

  EVT VT = LHS.getValueType();
  // FIXME: Handle jump negative someday
  SDValue TargetCC;
  TriCoreCC::CondCodes TCC = TriCoreCC::COND_INVALID;

  //outs() << "CC: " << (int)CC << "\n";

  switch (CC) {
  default: llvm_unreachable("Invalid integer condition!");
  case ISD::SETEQ:
    TCC = TriCoreCC::COND_EQ;     // aka COND_Z
    tricoreCC = DAG.getConstant(TriCoreCC::COND_NE, dl, MVT::i32);
    // Minor optimization: if LHS is a constant, swap operands, then the
    // constant can be folded into comparison.
    if (LHS.getOpcode() == ISD::Constant)
      std::swap(LHS, RHS);
    break;
  case ISD::SETNE:
    TCC = TriCoreCC::COND_NE;    // aka COND_NZ
    tricoreCC = DAG.getConstant(TriCoreCC::COND_NE, dl, MVT::i32);
    // Minor optimization: if LHS is a constant, swap operands, then the
    // constant can be folded into comparison.
    if (LHS.getOpcode() == ISD::Constant)
      std::swap(LHS, RHS);
    break;
  case ISD::SETULE:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETUGE:
    // Turn lhs u>= rhs with lhs constant into rhs u< lhs+1, this allows us to
    // fold constant into instruction.
    if (const ConstantSDNode * C = dyn_cast<ConstantSDNode>(LHS)) {
      LHS = RHS;
      RHS = DAG.getConstant(C->getSExtValue() + 1, dl, C->getValueType(0));
      TCC = TriCoreCC::COND_LT;
      break;
    }
    TCC = TriCoreCC::COND_GE;    // aka COND_C
    tricoreCC = DAG.getConstant(TriCoreCC::COND_NE, dl, MVT::i32);
    break;
  case ISD::SETUGT:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETULT:
    tricoreCC = DAG.getConstant(TriCoreCC::COND_NE, dl, MVT::i32);
    // Turn lhs u< rhs with lhs constant into rhs u>= lhs+1, this allows us to
    // fold constant into instruction.
    if (const ConstantSDNode * C = dyn_cast<ConstantSDNode>(LHS)) {
      LHS = RHS;
      RHS = DAG.getConstant(C->getSExtValue() + 1, dl, C->getValueType(0));
      TCC = TriCoreCC::COND_GE;
      break;
    }
    TCC = TriCoreCC::COND_LT;    // aka COND_NC
    break;
  case ISD::SETLE:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETGE:
    tricoreCC = DAG.getConstant(TriCoreCC::COND_NE, dl, MVT::i32);
    // Turn lhs >= rhs with lhs constant into rhs < lhs+1, this allows us to
    // fold constant into instruction.
    if (const ConstantSDNode * C = dyn_cast<ConstantSDNode>(LHS)) {
      LHS = RHS;
      RHS = DAG.getConstant(C->getSExtValue() + 1, dl, C->getValueType(0));
      TCC = TriCoreCC::COND_LT;
      break;
    }
    TCC = TriCoreCC::COND_GE;
    break;
  case ISD::SETGT:
    std::swap(LHS, RHS);        // FALLTHROUGH
  case ISD::SETLT:
    tricoreCC = DAG.getConstant(TriCoreCC::COND_NE, dl, MVT::i32);
    // Turn lhs < rhs with lhs constant into rhs >= lhs+1, this allows us to
    // fold constant into instruction.
    if (const ConstantSDNode * C = dyn_cast<ConstantSDNode>(LHS)) {
      LHS = RHS;
      RHS = DAG.getConstant(C->getSExtValue() + 1, dl, C->getValueType(0));
      TCC = TriCoreCC::COND_GE;
      break;
    }
    TCC = TriCoreCC::COND_LT;
    break;
  }

  if (VT == MVT::i64) {

    SDValue LHSlo = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, LHS,
                                   DAG.getIntPtrConstant(0, dl));
    SDValue LHShi = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, LHS,
                                   DAG.getIntPtrConstant(1, dl));

    SDValue RHSlo, RHShi;
    if (RHS.getOpcode() != ISD::Constant) {
        RHSlo = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, RHS,
                                           DAG.getIntPtrConstant(0, dl));
        RHShi = DAG.getNode(ISD::EXTRACT_ELEMENT, dl, MVT::i32, RHS,
                                           DAG.getIntPtrConstant(1, dl));
    }
    else
    {
      ConstantSDNode *C = cast<ConstantSDNode>(RHS);
      int64_t immVal = C->getSExtValue();
      int32_t lowerByte = immVal & 0xffffffff;
      int32_t HigherByte = (immVal >> 32);

      RHSlo = DAG.getConstant(lowerByte, dl, MVT::i32);
      RHShi = DAG.getConstant(HigherByte, dl, MVT::i32);
      // RHShi.dump();
    }

    SDValue TargetEQ;
    if (TCC != TriCoreCC::COND_NE)
      TargetEQ = DAG.getConstant(TriCoreCC::COND_EQ, dl, MVT::i32);
    else
      TargetEQ = DAG.getConstant(TCC, dl, MVT::i32);

    SDVTList VTs = DAG.getVTList(MVT::i32, MVT::Glue);
    SDValue Ops[] = {LHShi, RHShi, TargetEQ};
    SDValue compareHi = DAG.getNode(TRICOREISD::CMP, dl, VTs, Ops);

    TargetCC = DAG.getConstant(TCC, dl, MVT::i32);
    SDValue Ops2[] = {compareHi.getValue(0), LHSlo, RHSlo, TargetCC, compareHi.getValue(1)};
    SDValue compareLo = DAG.getNode(TRICOREISD::LOGICCMP, dl, VTs, Ops2);

    if ( (TCC == TriCoreCC::COND_NE ) || (TCC == TriCoreCC::COND_EQ))
      return compareLo;

    SDValue SecondCC = DAG.getConstant(TriCoreCC::COND_LT + 10, dl, MVT::i32);
    if(RHS.getOpcode() == ISD::Constant) {
      std::swap(LHShi, RHShi);
      SecondCC = DAG.getConstant(TCC + 10, dl, MVT::i32);
    }

    SDValue Ops3[] = {compareLo.getValue(0), RHShi, LHShi, SecondCC, compareLo.getValue(1)};
    SDValue compareCombine = DAG.getNode(TRICOREISD::LOGICCMP, dl, VTs, Ops3);

    return compareCombine;
    //LHS2.dump();
  }


  TargetCC = DAG.getConstant(TCC, dl, MVT::i32);
  SDVTList VTs = DAG.getVTList(MVT::i32, MVT::Glue);
  SDValue Ops[] = {LHS, RHS, TargetCC};

//  return DAG.getNode(TRICOREISD::CMP, dl, MVT::i32, LHS, RHS, TargetCC);



  return DAG.getNode(TRICOREISD::CMP, dl, VTs, Ops);
}

SDValue TriCoreTargetLowering::LowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(1))->get();
  SDValue LHS   = Op.getOperand(2);
  SDValue RHS   = Op.getOperand(3);
  SDValue Dest  = Op.getOperand(4);
  SDLoc dl  (Op);

  SDValue tricoreCC;
  SDValue Flag = EmitCMP(LHS, RHS, CC, dl, DAG, tricoreCC);

  //Flag.getValue(1).dump();

  return DAG.getNode(TRICOREISD::BR_CC, dl, Op.getValueType(),
                       Chain, Dest, Flag.getValue(0), tricoreCC, Flag.getValue(1));

}

SDValue TriCoreTargetLowering::LowerSETCC(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS   = Op.getOperand(0);
  SDValue RHS   = Op.getOperand(1);
  SDLoc dl  (Op);

  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(2))->get();
  SDValue TargetCC;
  SDValue Flag = EmitCMP(LHS, RHS, CC, dl, DAG, TargetCC);
  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue Ops[] = {LHS, RHS, TargetCC, Flag};

  return DAG.getNode(TRICOREISD::SELECT_CC, dl, VTs, Ops);

}


SDValue TriCoreTargetLowering::LowerSELECT_CC(SDValue Op,
                                             SelectionDAG &DAG) const {
  SDValue LHS    = Op.getOperand(0);
  SDValue RHS    = Op.getOperand(1);
  SDValue TrueV  = Op.getOperand(2);
  SDValue FalseV = Op.getOperand(3);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDLoc dl   (Op);

  SDValue tricoreCC;
  SDValue Flag = EmitCMP(LHS, RHS, CC, dl, DAG, tricoreCC);

  SDVTList VTs = DAG.getVTList(Op.getValueType(), MVT::Glue);
  SDValue Ops[] = {TrueV, FalseV, tricoreCC, Flag};

  return DAG.getNode(TRICOREISD::SELECT_CC, dl, VTs, Ops);
}

SDValue TriCoreTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG& DAG) const
{
  EVT VT = Op.getValueType();

  GlobalAddressSDNode *GlobalAddr = cast<GlobalAddressSDNode>(Op.getNode());
  int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();
  SDValue TargetAddr =
     DAG.getTargetGlobalAddress(GlobalAddr->getGlobal(), Op, MVT::i32, Offset);
  return DAG.getNode(TRICOREISD::Wrapper, Op, VT, TargetAddr);


//  EVT VT = Op.getValueType();
//  GlobalAddressSDNode *GlobalAddr = cast<GlobalAddressSDNode>(Op.getNode());
//  SDValue TargetAddr =
//      DAG.getTargetGlobalAddress(GlobalAddr->getGlobal(), Op, MVT::i32);
//  return DAG.getNode(TriCoreISD::Wrapper, Op, VT, TargetAddr);
}


MachineBasicBlock*
TriCoreTargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                  MachineBasicBlock *BB) const {
  unsigned Opc = MI.getOpcode();

  const TargetInstrInfo &TII = *BB->getParent()->getSubtarget().getInstrInfo();
  DebugLoc dl = MI.getDebugLoc();

  assert(Opc == TRICORE::Select8 && "Unexpected instr type to insert");
  // To "insert" a SELECT instruction, we actually have to insert the diamond
  // control-flow pattern.  The incoming instruction knows the destination vreg
  // to set, the condition code register to branch on, the true/false values to
  // select between, and a branch opcode to use.
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction::iterator I(BB);
  ++I;

  //  thisMBB:
  //  ...
  //   TrueVal = ...
  //   cmpTY ccX, r1, r2
  //   jCC copy1MBB
  //   fallthrough --> copy0MBB
  MachineBasicBlock *thisMBB = BB;
  MachineFunction *F = BB->getParent();
  MachineBasicBlock *copy0MBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *copy1MBB = F->CreateMachineBasicBlock(LLVM_BB);
  F->insert(I, copy0MBB);
  F->insert(I, copy1MBB);
  // Update machine-CFG edges by transferring all successors of the current
  // block to the new block which will contain the Phi node for the select.
  copy1MBB->splice(copy1MBB->begin(), BB,
                   std::next(MachineBasicBlock::iterator(MI)), BB->end());
  copy1MBB->transferSuccessorsAndUpdatePHIs(BB);
  // Next, add the true and fallthrough blocks as its successors.
  BB->addSuccessor(copy0MBB);
  BB->addSuccessor(copy1MBB);

  // MI.dump();

  BuildMI(BB, dl, TII.get(TRICORE::JNZsbr))
    .addMBB(copy1MBB)
    .addReg(MI.getOperand(4).getReg());

  //  copy0MBB:
  //   %FalseValue = ...
  //   # fallthrough to copy1MBB
  BB = copy0MBB;

  // Update machine-CFG edges
  BB->addSuccessor(copy1MBB);

  //  copy1MBB:
  //   %Result = phi [ %FalseValue, copy0MBB ], [ %TrueValue, thisMBB ]
  //  ...
  BB = copy1MBB;
  BuildMI(*BB, BB->begin(), dl, TII.get(TRICORE::PHI),
          MI.getOperand(0).getReg())
    .addReg(MI.getOperand(2).getReg()).addMBB(copy0MBB)
    .addReg(MI.getOperand(1).getReg()).addMBB(thisMBB);

  MI.eraseFromParent();   // The pseudo instruction is gone now.
  return BB;
}