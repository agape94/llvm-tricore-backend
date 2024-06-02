//===- TriCoreInstrFormats.td - TriCore Instruction Formats ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe TriCore instructions format
//
//  CPU INSTRUCTION FORMATS
//
//  op1     - primary operation code
//  op2     - secondary operation code
//  s1      - source register 1
//  s2      - source register 2
//  d       - destination register
//  n       - multiplication shift value (0b00 or 0b01)
//  const9  - 9 bits immediate value
//  disp24  - 24 bits displacement value
//
//===----------------------------------------------------------------------===//

class InstTriCore<dag outs, dag ins, string asmstr, list<dag> pattern>
    : Instruction {

  let Namespace = "TRICORE";
  /// outs and ins are inherited from the instruction class.
  dag OutOperandList = outs;
  dag InOperandList  = ins;
  let AsmString   = asmstr;
  let Pattern = pattern;
}

// TriCore pseudo instructions format
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
   : InstTriCore<outs, ins, asmstr, pattern> {
  let isPseudo = 1;
}

//===----------------------------------------------------------------------===//
// Generic 16-bit Instruction Format
//===----------------------------------------------------------------------===//
class T16<dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstTriCore<outs, ins, asmstr, pattern> {
  field bits<16> Inst;
  let Size = 2;
  field bits<16> SoftFail = 0;
}

//===----------------------------------------------------------------------===//
// Generic 32-bit Instruction Format
//===----------------------------------------------------------------------===//
class T32<dag outs, dag ins, string asmstr, list<dag> pattern>
    : InstTriCore<outs, ins, asmstr, pattern> {
  field bits<32> Inst;
  let Size = 4;
  field bits<32> SoftFail = 0;
}

//===----------------------------------------------------------------------===//
// 32-bit B Instruction Format:
// <disp24[15:0]|disp24[23:16]|op1>
//===----------------------------------------------------------------------===//
class B<bits<8> op1, dag outs, dag ins, string asmstr,
                 list<dag> pattern> : T32<outs, ins, asmstr, pattern> {

  bits<24> disp24;

  let Inst{31-16} = disp24{15-0};
  let Inst{15-8} = disp24{23-16};
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeBInstruction";
}

//===----------------------------------------------------------------------===//
// 32-bit RR Instruction Format: <d|op2|-|n|s2|s1|op1>
//===----------------------------------------------------------------------===//
class RR<bits<8> op1, bits<8> op2, dag outs, dag ins, string asmstr,
                 list<dag> pattern> : T32<outs, ins, asmstr, pattern> {

  bits<4> d;
  bits<2> n;
  bits<4> s2;
  bits<4> s1;

  let Inst{31-28} = d;
  let Inst{27-20} = op2;
  let Inst{19-18} = 0;
  let Inst{17-16} = n;
  let Inst{15-12} = s2;
  let Inst{11-8} = s1;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeRRInstruction";
}

//===----------------------------------------------------------------------===//
// 16-bit SRC Instruction Format: <const4|s1/d|op1>
//===----------------------------------------------------------------------===//
class SRC<bits<8> op1, dag outs, dag ins, string asmstr, list<dag> pattern>
    : T16<outs, ins, asmstr, pattern> {

  bits<4> const4;
  bits<4> s1_d;

  let Inst{15-12} = const4;
  let Inst{11-8} = s1_d;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeSRCInstruction";
}

//===----------------------------------------------------------------------===//
// 32-bit ABS Instruction Format:
// <off18[9:6]|op2|off18[13:10]|off18[5:0]|off18[17:14]|s1/d|op1>
//===----------------------------------------------------------------------===//
class ABS<bits<8> op1, bits<2> op2, dag outs, dag ins, string asmstr,
                 list<dag> pattern> : T32<outs, ins, asmstr, pattern> {

  bits<18> off18;
  bits<4> s1_d;

  let Inst{31-28} = off18{9-6};
  let Inst{27-26} = op2;
  let Inst{25-22} = off18{13-10};
  let Inst{21-16} = off18{5-0};
  let Inst{15-12} = off18{17-14};
  let Inst{11-8} = s1_d;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeABSInstruction";
}

//===----------------------------------------------------------------------===//
// 32-bit BO Instruction Format: <off10[9:6]|op2|off10[5:0]|s2|s1/d|op1>
//===----------------------------------------------------------------------===//
class BO<bits<8> op1, bits<6> op2, dag outs, dag ins, string asmstr,
                 list<dag> pattern> : T32<outs, ins, asmstr, pattern> {

  bits<10> off10;
  bits<4> s2;
  bits<4> s1_d;

  let Inst{31-28} = off10{9-6};
  let Inst{27-22} = op2;
  let Inst{21-16} = off10{5-0};
  let Inst{15-12} = s2;
  let Inst{11-8} = s1_d;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeBOInstruction";
}

//===----------------------------------------------------------------------===//
// 32-bit BOL Instruction Format:
// <off16[9:6]|off16[15:10]|off16[5:0]|s2|s1/d|op1>
//===----------------------------------------------------------------------===//
class BOL<bits<8> op1, dag outs, dag ins, string asmstr,
                 list<dag> pattern> : T32<outs, ins, asmstr, pattern> {

  bits<16> off16;
  bits<4> s2;
  bits<4> s1_d;

  let Inst{31-28} = off16{9-6};
  let Inst{27-22} = off16{15-10};
  let Inst{21-16} = off16{5-0};
  let Inst{15-12} = s2;
  let Inst{11-8} = s1_d;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeBOLInstruction";
}

//===----------------------------------------------------------------------===//
// 16-bit SB Instruction Format: <disp8|op1>
//===----------------------------------------------------------------------===//
class SB<bits<8> op1, dag outs, dag ins, string asmstr, list<dag> pattern>
        : T16<outs, ins, asmstr, pattern> {

  bits<8> disp8;
  let Inst{15-8} = disp8;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeSBInstruction";
}

//===----------------------------------------------------------------------===//
// 16-bit SBR Instruction Format: <s2|disp4|op1>
//===----------------------------------------------------------------------===//
class SBR<bits<8> op1, dag outs, dag ins, string asmstr, list<dag> pattern>
        : T16<outs, ins, asmstr, pattern> {

  bits<4> s2;
  bits<4> disp4;

  let Inst{15-12} = s2;
  let Inst{11-8} = disp4;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeSBRInstruction";
}

//===----------------------------------------------------------------------===//
// 32-bit RLC Instruction Format: <d|const16|s1|op1>
//===----------------------------------------------------------------------===//
class RLC<bits<8> op1, dag outs, dag ins, string asmstr,
                 list<dag> pattern> : T32<outs, ins, asmstr, pattern> {

  bits<4> d;
  bits<16> const16;
  bits<4> s1;

  let Inst{31-28} = d;
  let Inst{27-12} = const16;
  let Inst{11-8} = s1;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeRLCInstruction";
}

//===----------------------------------------------------------------------===//
// 16-bit SRR Instruction Format: <s2|s1/d|op1>
//===----------------------------------------------------------------------===//
class SRR<bits<8> op1, dag outs, dag ins, string asmstr, list<dag> pattern>
    : T16<outs, ins, asmstr, pattern> {

  bits<4> s2;
  bits<4> s1_d;

  let Inst{15-12} = s2;
  let Inst{11-8} = s1_d;
  let Inst{7-0} = op1;
  let DecoderMethod = "DecodeSRRInstruction";
}