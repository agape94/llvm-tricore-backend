//===-- TriCoreInstrInfo.td - Target Description for TriCore Target -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the TriCore instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "TriCoreInstrFormats.td"

// -------------------------------------------------- //
// Instruction Operands and Patterns
// -------------------------------------------------- //

//  These are target-independent nodes, but have target-specific formats.
def SDT_TriCoreCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>,
                                            SDTCisVT<1, i32>]>;
def SDT_TriCoreCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>,
                                          SDTCisVT<1, i32>]>;

def SDT_TriCoreCall : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;
def SDT_TriCoreWrapper  : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,
                           SDTCisPtrTy<0>]>;
def SDT_TriCoreMovei32         : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,
                           SDTCisVT<0, i32>]>;
def SDT_TriCoreBrCC     : SDTypeProfile<0, 3, [SDTCisVT<0, OtherVT>,
                           SDTCisVT<1, i32>,
                           SDTCisVT<2, i32>]>;
def SDT_TriCoreImask    : SDTypeProfile<1, 3, [SDTCisVT<0, i64>,
                           SDTCisVT<1, i32>,
                           SDTCisVT<2, i32>,
                           SDTCisVT<3, i32>]>;

def TriCoreCall    : SDNode<"TRICOREISD::CALL", SDT_TriCoreCall,
             [ SDNPHasChain, SDNPOptInGlue, SDNPOutGlue, SDNPVariadic ]>;
def TriCoreRetFlag    : SDNode<"TRICOREISD::RET_FLAG", SDTNone,
                           [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def TriCoreBrCC    : SDNode<"TRICOREISD::BR_CC",
              SDT_TriCoreBrCC, [SDNPHasChain, SDNPInGlue]>;
def CallSeqStart : SDNode<"ISD::CALLSEQ_START", SDT_TriCoreCallSeqStart,
                           [SDNPHasChain, SDNPOutGlue, SDNPSideEffect]>;
def CallSeqEnd   : SDNode<"ISD::CALLSEQ_END",   SDT_TriCoreCallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue,
                           SDNPSideEffect]>;

let hasSideEffects = 1, Namespace="TRICORE" in {
def ADJCALLSTACKDOWN : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                    "# ADJCALLSTACKDOWN $amt1, $amt2",
                                    [(CallSeqStart timm:$amt1, timm:$amt2)]>;
def ADJCALLSTACKUP   : Pseudo<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                                    "# ADJCALLSTACKUP $amt1",
                                    [(CallSeqEnd timm:$amt1, timm:$amt2)]>;
}

def movei32 : SDNode<"TRICOREISD::MOVEi32", SDT_TriCoreMovei32>;

def TriCoreWrapper : SDNode<"TRICOREISD::Wrapper", SDT_TriCoreWrapper>;
def TriCoreAbs     : SDNode<"TRICOREISD::ABS", SDTIntUnaryOp>;
def TriCoreImask   : SDNode<"TRICOREISD::IMASK", SDT_TriCoreImask>;

def jmptarget : Operand<OtherVT> {
  let PrintMethod = "printPCRelImmOperand";
}

def isPointer : Predicate<"isPointer() == true">;
def isnotPointer : Predicate<"isPointer() == false">;

// TriCore Condition Codes
def TriCore_COND_EQ : PatLeaf<(i32 0)>;
def TriCore_COND_NE : PatLeaf<(i32 1)>;

let isReturn = 1, isTerminator = 1, hasDelaySlot = 1, isBarrier = 1 in{
  def RET : T32<(outs), (ins variable_ops), "ret",  [(TriCoreRetFlag)]>;
}

// The target of a 24-bit call instruction.
def call_target : Operand<i32>
{
  let EncoderMethod = "encodeCallTarget";
}


let isCall = 1, Defs = [A11], Uses = [A10] in
 def CALLb : B<0x6D, (outs), (ins i32imm:$disp24),
 "call $disp24",  [(TriCoreCall imm:$disp24)]>;

def MOVi32 : Pseudo<(outs DataRegs:$d), (ins i32imm:$const32), "##NAME## Pseudo",
                     [(set DataRegs:$d, (movei32 imm:$const32))]>;

def : Pat<(TriCoreCall (i32 tglobaladdr:$dst)),
     (CALLb tglobaladdr:$dst)>;
def : Pat<(i32 (TriCoreWrapper tglobaladdr:$dst)),
   (MOVi32 tglobaladdr:$dst)>;

//Operands
def s4imm      : Operand<i32> { let PrintMethod = "printSExtImm<4>";  }
def s6imm      : Operand<i32> { let PrintMethod = "printSExtImm<6>";  }
def s9imm      : Operand<i32> { let PrintMethod = "printSExtImm<9>";  }
def s16imm     : Operand<i32> { let PrintMethod = "printSExtImm<16>"; }
def s24imm     : Operand<i32> { let PrintMethod = "printSExtImm<24>"; }
def u8imm      : Operand<i32> { let PrintMethod = "printZExtImm<8>";  }
def u4imm      : Operand<i32> { let PrintMethod = "printZExtImm<4>";  }
def u9imm      : Operand<i32> { let PrintMethod = "printZExtImm<9>";  }
def u16imm     : Operand<i32> { let PrintMethod = "printZExtImm<16>";  }

def immSExt4  : PatLeaf<(imm), [{ return isInt<4>(N->getSExtValue()); }]>;
def immSExt6  : PatLeaf<(imm), [{ return isInt<6>(N->getSExtValue()); }]>;
def immSExt9  : PatLeaf<(imm), [{ return isInt<9>(N->getSExtValue()); }]>;
def immSExt16  : PatLeaf<(imm), [{ return isInt<16>(N->getSExtValue()); }]>;
def immSExt24  : PatLeaf<(imm), [{ return isInt<24>(N->getSExtValue()); }]>;

def immZExt4 : ImmLeaf<i32, [{return Imm == (Imm & 0xf);}]>;
def immZExt8 : ImmLeaf<i32, [{return Imm == (Imm & 0xff);}]>;
def immZExt9 : ImmLeaf<i32, [{return Imm == (Imm & 0x1ff);}]>;
def immZExt16 : ImmLeaf<i32, [{return Imm == (Imm & 0xffff);}]>;

def PairAddrRegsOp : RegisterOperand<PairAddrRegs, "printPairAddrRegsOperand">;

def bo : Operand<i32> {
  let MIOperandInfo = (ops AddrRegs, i32imm);
  let PrintMethod = "printAddrBO";
  let EncoderMethod = "getBaseOffsetValue";
}

def preincbo : Operand<i32> {
  let MIOperandInfo = (ops AddrRegs, i32imm);
  let PrintMethod = "printAddrPreIncBO";
  let EncoderMethod = "getBaseOffsetValue";
}

def postincbo : Operand<i32> {
  let MIOperandInfo = (ops AddrRegs, i32imm);
  let PrintMethod = "printAddrPostIncBO";
  let EncoderMethod = "getBaseOffsetValue";
}

def circbo : Operand<i32> {
  let MIOperandInfo = (ops PairAddrRegsOp, i32imm);
  let PrintMethod = "printAddrCircBO";
  let EncoderMethod = "getBaseOffsetValue";
}

// FIXME: Bit-reverse addressing mode has no offset
// Immediate operand is ignored in PrintMethod and EncoderMethod
def bitrevbo : Operand<i32> {
  let MIOperandInfo = (ops PairAddrRegsOp, i32imm);
  let PrintMethod = "printAddrBitRevBO";  
  let EncoderMethod = "getBaseValue";
}

def imml_32_h_32_const9 : Operand<i64>, PatLeaf<(imm), [{

 int64_t val = N->getSExtValue();
 int32_t lByte = val & 0xffffffff;
 int32_t hByte = (val >> 32) & 0xffffffff;

 return (lByte >=0 && lByte < 512 && hByte >=0 && hByte < 512);

}]>;

// Addressing modes
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;

//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//

// Absolute Value Instructions
let Defs = [PSW] in {
   def ABS : RR<0x0B, 0x1C, (outs DataRegs:$d),
                  (ins DataRegs:$s2), "abs $d, $s2",
                  [(set DataRegs:$d, (TriCoreAbs DataRegs:$s2))]>;
}

// Logical Instructions
multiclass Logical32_0<bits<8> opcode1, string asmstring,
SDNode OpNode>
{
 def rc: RC<0x8f, opcode1{6-0}, (outs DataRegs:$d),
   (ins DataRegs:$s1, u9imm:$const9),
   !strconcat(asmstring, " $d, $s1, $const9"),
   [(set DataRegs:$d, (OpNode DataRegs:$s1, immZExt9:$const9))]>;

 let isCommutable = 1 in
   def rr: RR<0x0f, opcode1, (outs DataRegs:$d),
   (ins DataRegs:$s1, DataRegs:$s2),
   !strconcat(asmstring, " $d, $s1, $s2"),
   [(set DataRegs:$d, (OpNode DataRegs:$s1, DataRegs:$s2))]>;
}

multiclass Logical32_1<bits<8> opcode1, string asmstring,
SDNode OpNode1, PatFrag OpNode2>
{
 def rc: RC<0x8f, opcode1{6-0}, (outs DataRegs:$d),
   (ins DataRegs:$s1, u9imm:$const9),
   !strconcat(asmstring, " $d, $s1, $const9"),
   [(set DataRegs:$d, (OpNode2 (OpNode1 DataRegs:$s1, immZExt9:$const9)))]>;

 let isCommutable = 1 in
   def rr: RR<0x0f, opcode1, (outs DataRegs:$d),
   (ins DataRegs:$s1, DataRegs:$s2),
   !strconcat(asmstring, " $d, $s1, $s2"),
   [(set DataRegs:$d, (OpNode2 (OpNode1 DataRegs:$s1, DataRegs:$s2)))]>;
}

multiclass Logical16<bits<8> opcode1_sc, bits<8> opcode1_srr,
string asmstring, SDNode OpNode>
{

 let Uses=[D15], Defs=[D15] in
   def sc: SC<opcode1_sc, (outs),
     (ins u8imm:$const8),
     !strconcat(asmstring, " %d15, $const8"),
     [(set D15, (OpNode D15, immZExt8:$const8))]>;

 let Constraints = "$s1 = $d" in
   def srr: SRR<opcode1_srr, (outs DataRegs:$d),
   (ins DataRegs:$s1, DataRegs:$s2),
   !strconcat(asmstring, " $d, $s2"),
   [(set DataRegs:$d, (OpNode DataRegs:$s1, DataRegs:$s2))]>;
}

multiclass Logical64_Pseudo<SDNode OpNode>
{
 def rc64: Pseudo<(outs DataExtRegs:$d),
   (ins DataExtRegs:$s1, imml_32_h_32_const9:$const64),
   "##NAME## Pseudo",
   [(set DataExtRegs:$d, (OpNode DataExtRegs:$s1, imml_32_h_32_const9:$const64))]>;

 let Constraints = "$s1 = $d", isCommutable = 1 in
   def srr64: Pseudo<(outs DataExtRegs:$d),
   (ins DataExtRegs:$s1, DataExtRegs:$s2),
   "##NAME## Pseudo",
   [(set DataExtRegs:$d, (OpNode DataExtRegs:$s1, DataExtRegs:$s2))]>;
}

defm AND : Logical64_Pseudo<and>;
defm OR  : Logical64_Pseudo<or>;
defm XOR : Logical64_Pseudo<xor>;

defm AND : Logical16<0x16, 0x26, "and", and>;
defm AND : Logical32_0<0x08, "and", and>;

defm NAND : Logical32_1<0x09, "nand", and, not>;
defm NOR  : Logical32_1<0x0B, "nor", or, not>;
//defm XNOR : Logical32_1<0x0B, "nxor", xor, not>;

defm OR : Logical32_0<0x0a, "or", or>;
defm OR : Logical16<0x96, 0xA6, "or", or>;

defm XOR : Logical32_0<0x0c, "xor", xor>;

//===----------------------------------------------------------------------===//
// Arithmetic Instructions
let Constraints = "$s1 = $d" in
  def ADDsrc : SRC<0xC2, (outs DataRegs:$d), (ins DataRegs:$s1, s4imm:$const4),
  "add $d, $const4",
  [(set DataRegs:$d, (add DataRegs:$s1, immSExt4:$const4) )]>;

let Constraints="$d = $fksrc",
  AddedComplexity = 7 in
def ADDsrr: SRR<0x42, (outs DataRegs:$d),
  (ins DataRegs:$fksrc, DataRegs:$s2 ),
  "add $d, $s2",
  [(set DataRegs:$d, (add DataRegs:$s2, DataRegs:$fksrc))]>;

def ADDrc : RC<0x8B, 0x00, (outs DataRegs:$d),
  (ins DataRegs:$s1, i32imm:$const9),
  "add $d, $s1, $const9",
  [(set DataRegs:$d, (add DataRegs:$s1, immSExt9:$const9))]>;

def ADDIrlc : RLC<0x1B, (outs DataRegs:$d),
  (ins DataRegs:$s1, i32imm:$const16), "addi $d, $s1, $const16",
  [(set DataRegs:$d, (add DataRegs:$s1, immSExt16:$const16))]>;

let isCommutable = 1 in {

 let AddedComplexity = 6 in
 def ADDrr : RR<0x0B, 0x00, (outs DataRegs:$d),
   (ins DataRegs:$s1, DataRegs:$s2),
   "add $d, $s1, $s2",
   [(set i32:$d, (add i32:$s1, i32:$s2))]>;

 def ADD_Arr : RR<0x01, 0x01, (outs AddrRegs:$d),
   (ins AddrRegs:$s1, AddrRegs:$s2),
   "add.a $d, $s1, $s2",
   [(set AddrRegs:$d, (add AddrRegs:$s1, AddrRegs:$s2))]>;

} // let isCommutable = 1

//===----------------------------------------------------------------------===//
// Move Immediate Instructions
class MOV_RR<bits<8> op1, bits<8> op2, string opstr,
RegisterClass outregClass, RegisterClass inregClass>
: RR<op1, op2,
 (outs outregClass:$d),
 (ins inregClass:$s2),
 !strconcat(opstr, " $d, $s2"), [/* No Pattern*/]>;

class MOV_CONST<bits<8> opcode, string opstr, dag ins, list<dag> pattern>
: RLC<opcode, (outs DataRegs:$d), ins,
 !strconcat(opstr, " $d, $const16"), pattern>;

def MOVrr   : MOV_RR<0x0B, 0x1F, "mov", DataRegs, DataRegs>;
def MOV_Drr  : MOV_RR<0x01, 0x4C, "mov.d", DataRegs, AddrRegs>;
def MOV_Arr  : MOV_RR<0x01, 0x63, "mov.a", AddrRegs, DataRegs>;

def MOV_AAsrr : SRR<0x40, (outs AddrRegs:$d),
            (ins AddrRegs:$s2),
            "mov.aa $d, $s2", [] >;

def MOV_AArr : MOV_RR<0x01, 0x00, "mov.aa", AddrRegs, AddrRegs>;


def MOVsrc : SRC<0x82, (outs DataRegs:$d),
        (ins s4imm:$const4),
        "mov $d, $const4",
         [(set DataRegs:$d, immSExt4:$const4)]>;

def MOVrlc  : MOV_CONST<0x3B,"mov", (ins s16imm:$const16),
              [(set DataRegs:$d, immSExt16:$const16)]>;
def MOV_Urlc : MOV_CONST<0xBB,"mov.u", (ins u16imm:$const16),
              [(set DataRegs:$d, immZExt16:$const16)]>;

def MOVHrlc : MOV_CONST<0x7B, "movh", (ins i32imm:$const16), [/* No Pattern*/]>;

//===----------------------------------------------------------------------===//
// Load/Store Instructions
class LoadBO<bits<6> op2, string opstr, PatFrag PF,
         RegisterClass RC = DataRegs, ValueType intType = i32>
        : BO<0x09, op2, (outs RC:$d),
         (ins bo:$memri),
         !strconcat(opstr, " $d, $memri"),
         [(set RC:$d, (intType (PF addr:$memri)))]>{ let mayLoad = 1; }

def LD_Bbo  : LoadBO<0x20, "ld.b"  , sextloadi8>;
def LD_BUbo : LoadBO<0x21, "ld.bu" , zextloadi8>;
def LD_Hbo  : LoadBO<0x22, "ld.h"  , sextloadi16>;
def LD_HUbo : LoadBO<0x23, "ld.hu" , zextloadi16>;
def LD_Wbo  : LoadBO<0x24, "ld.w"  , load>;
def LD_Dbo  : LoadBO<0x25, "ld.d"  , load, DataExtRegs, i64>;
def LD_Abo  : LoadBO<0x26, "ld.a"  , load, AddrRegs>;
def LD_DAbo : BO<0x09, 0x27, (outs PairAddrRegsOp:$d),
         (ins bo:$memri), "ld.da $d, $memri",
         [(set PairAddrRegsOp:$d, (i64 (load addr:$memri)))]>{ let mayLoad = 1; }
def LD_Qbo  : LoadBO<0x28, "ld.q"  , load>;

class LoadPreIncBO<bits<6> op2, string opstr, PatFrag PF,
         RegisterClass RC = DataRegs, ValueType intType = i32>
        : BO<0x09, op2, (outs RC:$d),
         (ins preincbo:$memri),
         !strconcat(opstr, " $d, $memri"),
         [(set RC:$d, (intType (PF addr:$memri)))]>{ let mayLoad = 1; }

def LD_Bpreincbo  : LoadPreIncBO<0x10, "ld.b"  , sextloadi8>;
def LD_BUpreincbo : LoadPreIncBO<0x11, "ld.bu" , zextloadi8>;
def LD_Hpreincbo  : LoadPreIncBO<0x12, "ld.h"  , sextloadi16>;
def LD_HUpreincbo : LoadPreIncBO<0x13, "ld.hu" , zextloadi16>;
def LD_Wpreincbo  : LoadPreIncBO<0x14, "ld.w"  , load>;
def LD_Dpreincbo  : LoadPreIncBO<0x15, "ld.d"  , load, DataExtRegs, i64>;
def LD_Apreincbo  : LoadPreIncBO<0x16, "ld.a"  , load, AddrRegs>;
def LD_DApreincbo : BO<0x09, 0x17, (outs PairAddrRegsOp:$d),
         (ins preincbo:$memri),
         "ld.da $d, $memri",
         [(set PairAddrRegsOp:$d, (i64 (load addr:$memri)))]>{ let mayLoad = 1; }
def LD_Qpreincbo  : LoadPreIncBO<0x18, "ld.q"  , load>;

class LoadPostIncBO<bits<6> op2, string opstr, PatFrag PF,
         RegisterClass RC = DataRegs, ValueType intType = i32>
        : BO<0x09, op2, (outs RC:$d),
         (ins postincbo:$memri),
         !strconcat(opstr, " $d, $memri"),
         [(set RC:$d, (intType (PF addr:$memri)))]>{ let mayLoad = 1; }

def LD_Bpostincbo  : LoadPostIncBO<0x00, "ld.b"  , sextloadi8>;
def LD_BUpostincbo : LoadPostIncBO<0x01, "ld.bu" , zextloadi8>;
def LD_Hpostincbo  : LoadPostIncBO<0x02, "ld.h"  , sextloadi16>;
def LD_HUpostincbo : LoadPostIncBO<0x03, "ld.hu" , zextloadi16>;
def LD_Wpostincbo  : LoadPostIncBO<0x04, "ld.w"  , load>;
def LD_Dpostincbo  : LoadPostIncBO<0x05, "ld.d"  , load, DataExtRegs, i64>;
def LD_Apostincbo  : LoadPostIncBO<0x06, "ld.a"  , load, AddrRegs>;
def LD_DApostincbo : BO<0x09, 0x07, (outs PairAddrRegsOp:$d),
         (ins postincbo:$memri),
         "ld.da $d, $memri",
         [(set PairAddrRegsOp:$d, (i64 (load addr:$memri)))]>{ let mayLoad = 1; }
def LD_Qpostincbo  : LoadPostIncBO<0x08, "ld.q"  , load>;

class LoadCircBO<bits<6> op2, string opstr, PatFrag PF,
         RegisterClass RC = DataRegs, ValueType intType = i32>
        : BO<0x29, op2, (outs RC:$d),
         (ins circbo:$memri),
         !strconcat(opstr, " $d, $memri"),
         [(set RC:$d, (intType (PF addr:$memri)))]>{ let mayLoad = 1; }

def LD_Bcircbo  : LoadCircBO<0x10, "ld.b"  , sextloadi8>;
def LD_BUcircbo : LoadCircBO<0x11, "ld.bu" , zextloadi8>;
def LD_Hcircbo  : LoadCircBO<0x12, "ld.h"  , sextloadi16>;
def LD_HUcircbo : LoadCircBO<0x13, "ld.hu" , zextloadi16>;
def LD_Wcircbo  : LoadCircBO<0x14, "ld.w"  , load>;
def LD_Dcircbo  : LoadCircBO<0x15, "ld.d"  , load, DataExtRegs, i64>;
def LD_Acircbo  : LoadCircBO<0x16, "ld.a"  , load, AddrRegs>;
def LD_DAcircbo : BO<0x29, 0x17, (outs PairAddrRegsOp:$d),
         (ins circbo:$memri),
         "ld.da $d, $memri",
         [(set PairAddrRegsOp:$d, (i64 (load addr:$memri)))]>{ let mayLoad = 1; }
def LD_Qcircbo  : LoadCircBO<0x18, "ld.q"  , load>;

class LoadBitRevBO<bits<6> op2, string opstr, PatFrag PF,
         RegisterClass RC = DataRegs, ValueType intType = i32>
        : BO<0x29, op2, (outs RC:$d),
         (ins bitrevbo:$memri),
         !strconcat(opstr, " $d, $memri"),
         [(set RC:$d, (intType (PF addr:$memri)))]>{ let mayLoad = 1; }

def LD_Bbitrevbo  : LoadBitRevBO<0x00, "ld.b"  , sextloadi8>;
def LD_BUbitrevbo : LoadBitRevBO<0x01, "ld.bu" , zextloadi8>;
def LD_Hbitrevbo  : LoadBitRevBO<0x02, "ld.h"  , sextloadi16>;
def LD_HUbitrevbo : LoadBitRevBO<0x03, "ld.hu" , zextloadi16>;
def LD_Wbitrevbo  : LoadBitRevBO<0x04, "ld.w"  , load>;
def LD_Dbitrevbo  : LoadBitRevBO<0x05, "ld.d"  , load, DataExtRegs, i64>;
def LD_Abitrevbo  : LoadBitRevBO<0x06, "ld.a"  , load, AddrRegs>;
def LD_DAbitrevbo : BO<0x29, 0x07, (outs PairAddrRegsOp:$d),
         (ins bitrevbo:$memri),
         "ld.da $d, $memri",
         [(set PairAddrRegsOp:$d, (i64 (load addr:$memri)))]>{ let mayLoad = 1; }
def LD_Qbitrevbo  : LoadBitRevBO<0x08, "ld.q"  , load>;

def LD_Wbol : BOL<0x19, (outs DataRegs:$d),
   (ins bo:$memri),
   "ld.w $d, $memri",
   [(set DataRegs:$d, (load addr:$memri))]>{ let mayLoad = 1; }
def LD_Abol : BOL<0x99, (outs AddrRegs:$d),
   (ins bo:$memri),
   "ld.a $d, $memri",
   [(set AddrRegs:$d, (load addr:$memri))]>{ let mayLoad = 1; }

def : Pat<(extloadi8 addr:$src), (LD_Bbo addr:$src)>;
def : Pat<(extloadi16 addr:$src), (LD_Hbo addr:$src)>;

let Predicates = [isnotPointer] in {
 def ST_Babs : ABS<0x25, 0x00,(outs), (ins DataRegs:$d, i32imm:$memri),
   "st.b $memri, $d",
   [(truncstorei8 DataRegs:$d, imm:$memri)]>;
 def ST_Habs : ABS<0x25, 0x02,(outs), (ins DataRegs:$d, i32imm:$memri),
   "st.h $memri, $d",
   [(truncstorei16 DataRegs:$d, imm:$memri)]>;
 def ST_Wabs : ABS<0xA5, 0x00, (outs), (ins DataRegs:$d, i32imm:$memri),
   "st.w $memri, $d",
   [(store DataRegs:$d, imm:$memri)]>;
 def ST_Dabs : ABS<0xA5, 0x01, (outs), (ins DataExtRegs:$d, i32imm:$memri),
   "st.d $memri, $d",
   [(store DataExtRegs:$d, imm:$memri)]>;
 def ST_Qabs : ABS<0x65, 0x00, (outs), (ins DataRegs:$d, i32imm:$memri),
   "st.q $memri, $d",
   [(store DataRegs:$d, imm:$memri)]>;

 def ST_Bbo : BO<0x89, 0x20,(outs), (ins DataRegs:$d, bo:$memri),
   "st.b $memri, $d",
   [(truncstorei8 DataRegs:$d, addr:$memri)]>;
 def ST_Hbo : BO<0x89, 0x22,(outs), (ins DataRegs:$d, bo:$memri),
   "st.h $memri, $d",
   [(truncstorei16 DataRegs:$d, addr:$memri)]>;
 def ST_Wbo : BO<0x89, 0x24, (outs), (ins DataRegs:$d, bo:$memri),
   "st.w $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;
 def ST_Dbo : BO<0x89, 0x25, (outs), (ins DataExtRegs:$d, bo:$memri),
   "st.d $memri, $d",
   [(store DataExtRegs:$d, addr:$memri)]>;
 def ST_Qbo : BO<0x89, 0x28, (outs), (ins DataRegs:$d, bo:$memri),
   "st.q $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;

 def ST_Bpreincbo : BO<0x89, 0x10,(outs), (ins DataRegs:$d, preincbo:$memri),
   "st.b $memri, $d",
   [(truncstorei8 DataRegs:$d, addr:$memri)]>;
 def ST_Hpreincbo : BO<0x89, 0x12,(outs), (ins DataRegs:$d, preincbo:$memri),
   "st.h $memri, $d",
   [(truncstorei16 DataRegs:$d, addr:$memri)]>;
 def ST_Wpreincbo : BO<0x89, 0x14, (outs), (ins DataRegs:$d, preincbo:$memri),
   "st.w $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;
 def ST_Dpreincbo : BO<0x89, 0x15, (outs), (ins DataExtRegs:$d, preincbo:$memri),
   "st.d $memri, $d",
   [(store DataExtRegs:$d, addr:$memri)]>;
 def ST_Qpreincbo : BO<0x89, 0x18, (outs), (ins DataRegs:$d, preincbo:$memri),
   "st.q $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;

 def ST_Bpostincbo : BO<0x89, 0x00,(outs), (ins DataRegs:$d, postincbo:$memri),
   "st.b $memri, $d",
   [(truncstorei8 DataRegs:$d, addr:$memri)]>;
 def ST_Hpostincbo : BO<0x89, 0x02,(outs), (ins DataRegs:$d, postincbo:$memri),
   "st.h $memri, $d",
   [(truncstorei16 DataRegs:$d, addr:$memri)]>;
 def ST_Wpostincbo : BO<0x89, 0x04, (outs), (ins DataRegs:$d, postincbo:$memri),
   "st.w $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;
 def ST_Dpostincbo : BO<0x89, 0x05, (outs), (ins DataExtRegs:$d, postincbo:$memri),
   "st.d $memri, $d",
   [(store DataExtRegs:$d, addr:$memri)]>;
 def ST_Qpostincbo : BO<0x89, 0x08, (outs), (ins DataRegs:$d, postincbo:$memri),
   "st.q $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;

 def ST_Bcircbo : BO<0xA9, 0x10,(outs), (ins DataRegs:$d, circbo:$memri),
   "st.b $memri, $d",
   [(truncstorei8 DataRegs:$d, addr:$memri)]>;
 def ST_Hcircbo : BO<0xA9, 0x12,(outs), (ins DataRegs:$d, circbo:$memri),
   "st.h $memri, $d",
   [(truncstorei16 DataRegs:$d, addr:$memri)]>;
 def ST_Wcircbo : BO<0xA9, 0x14, (outs), (ins DataRegs:$d, circbo:$memri),
   "st.w $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;
 def ST_Dcircbo : BO<0xA9, 0x15, (outs), (ins DataExtRegs:$d, circbo:$memri),
   "st.d $memri, $d",
   [(store DataExtRegs:$d, addr:$memri)]>;
 def ST_Qcircbo : BO<0xA9, 0x18, (outs), (ins DataRegs:$d, circbo:$memri),
   "st.q $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;

 def ST_Bbitrevbo : BO<0xA9, 0x00,(outs), (ins DataRegs:$d, bitrevbo:$memri),
   "st.b $memri, $d",
   [(truncstorei8 DataRegs:$d, addr:$memri)]>;
 def ST_Hbitrevbo : BO<0xA9, 0x02,(outs), (ins DataRegs:$d, bitrevbo:$memri),
   "st.h $memri, $d",
   [(truncstorei16 DataRegs:$d, addr:$memri)]>;
 def ST_Wbitrevbo : BO<0xA9, 0x04, (outs), (ins DataRegs:$d, bitrevbo:$memri),
   "st.w $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;
 def ST_Dbitrevbo : BO<0xA9, 0x05, (outs), (ins DataExtRegs:$d, bitrevbo:$memri),
   "st.d $memri, $d",
   [(store DataExtRegs:$d, addr:$memri)]>;
 def ST_Qbitrevbo : BO<0xA9, 0x08, (outs), (ins DataRegs:$d, bitrevbo:$memri),
   "st.q $memri, $d",
   [(store DataRegs:$d, addr:$memri)]>;

 def : Pat<(truncstorei32 DataExtRegs:$d, addr:$memri),
    (ST_Wbo (EXTRACT_SUBREG DataExtRegs:$d, subreg_even), addr:$memri)>;

 def : Pat<(truncstorei8 DataExtRegs:$d, addr:$memri),
     (ST_Wbo (ANDrc (EXTRACT_SUBREG DataExtRegs:$d, subreg_even),
     (i32 255)), addr:$memri)>;
} // let Predicates = [isnotPointer]


let Predicates = [isPointer] in {
  def ST_Aabs : ABS<0xA5, 0x02,(outs), (ins AddrRegs:$d, i32imm:$memri),
   "st.a $memri, $d",
   [(truncstorei8 i32:$d, imm:$memri)]>;

  def ST_DAabs : ABS<0xA5, 0x03,(outs), (ins PairAddrRegsOp:$d, i32imm:$memri),
   "st.da $memri, $d",
   [(truncstorei8 i64:$d, imm:$memri)]>;

  def ST_Abo : BO<0x89, 0x26, (outs), (ins AddrRegs:$d, bo:$memri),
  "st.a $memri, $d",
  [(store i32:$d, addr:$memri)]>;

  def ST_DAbo : BO<0x89, 0x27, (outs), (ins PairAddrRegsOp:$d, bo:$memri),
  "st.da $memri, $d",
  [(store i64:$d, addr:$memri)]>;

  def ST_Apreincbo : BO<0x89, 0x16,(outs), (ins AddrRegs:$d, preincbo:$memri),
   "st.a $memri, $d",
   [(store i32:$d, addr:$memri)]>;

  def ST_DApreincbo : BO<0x89, 0x17,(outs), (ins PairAddrRegsOp:$d, preincbo:$memri),
   "st.da $memri, $d",
   [(store i64:$d, addr:$memri)]>;

   def ST_Apostincbo : BO<0x89, 0x06,(outs), (ins AddrRegs:$d, postincbo:$memri),
   "st.a $memri, $d",
   [(store i32:$d, addr:$memri)]>;

   def ST_DApostincbo : BO<0x89, 0x07,(outs), (ins PairAddrRegsOp:$d, postincbo:$memri),
   "st.da $memri, $d",
   [(store i64:$d, addr:$memri)]>;
   
   def ST_Acircbo : BO<0xA9, 0x16,(outs), (ins AddrRegs:$d, circbo:$memri),
   "st.a $memri, $d",
   [(store i32:$d, addr:$memri)]>;

   def ST_DAcircbo : BO<0xA9, 0x17,(outs), (ins PairAddrRegsOp:$d, circbo:$memri),
   "st.da $memri, $d",
   [(store i64:$d, addr:$memri)]>;

   def ST_Abitrevbo : BO<0xA9, 0x06,(outs), (ins AddrRegs:$d, bitrevbo:$memri),
   "st.a $memri, $d",
   [(store i32:$d, addr:$memri)]>;

   def ST_DAbitrevbo : BO<0xA9, 0x07,(outs), (ins PairAddrRegsOp:$d, bitrevbo:$memri),
   "st.da $memri, $d",
   [(store i64:$d, addr:$memri)]>;

} // let Predicates = [isPointer]
//===----------------------------------------------------------------------===//
// Branching instructions

// Branch Instructions
let Uses = [PC], Defs = [PC] in
multiclass JUMP_16<bits<8> op1_sb, bits<8> op1_sbr,
         string asmstring, PatLeaf PF>
{
  let Uses = [D15] in
  def sb: SB<op1_sb, (outs),
     (ins jmptarget:$disp8),
     !strconcat(asmstring, " %d15, $disp8"),
     [(TriCoreBrCC bb:$disp8, D15, PF)]>;

  def sbr: SBR<op1_sbr, (outs),
     (ins DataRegs:$s1, jmptarget:$disp4),
     !strconcat(asmstring, " $s1, $disp4"),
     [(TriCoreBrCC bb:$disp4, DataRegs:$s1, PF)]>;
}

let isBranch = 1, isTerminator = 1 in {
// Direct branch
let isBarrier = 1 in {
  def Jb : B<0x1D, (outs), (ins jmptarget:$dst),
                "j $dst",
        [(br bb:$dst)]>;
}

// Conditional branches

 defm JNZ : JUMP_16<0xEE, 0xF6, "jnz", TriCore_COND_NE>;
 defm JZ : JUMP_16<0x6E, 0x76, "jz", TriCore_COND_EQ>;

} // isBranch, isTerminator

//===----------------------------------------------------------------------===//

def IMASKrcpw :  RCPW<0xB7, 0b01, (outs DataExtRegs:$d),
  (ins u4imm:$const4, i32imm:$pos, i32imm:$width),
  "imask $d, $const4, $pos, $width",
  [(set DataExtRegs:$d, (TriCoreImask immZExt4:$const4, imm:$pos, imm:$width))]>;