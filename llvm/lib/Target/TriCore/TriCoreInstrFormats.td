//===- TriCoreInstrFormats.td - TriCore Instruction Formats ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class TriCoreInst<dag outs, dag ins, string asm, list<dag> pattern = []>
    : Instruction {
  bits<32> Inst;
  bits<32> SoftFail = 0;
  let Namespace = "TRICORE";
  let Size = 4;
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asm;
  let Pattern = pattern;
  let DecoderNamespace = "TRICORE";
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, list<dag> pattern>
  : TriCoreInst<outs, ins, "", pattern> {
  let isCodeGenOnly       = 1;
  let isPseudo            = 1;
  let hasNoSchedulingInfo = 1;
}
