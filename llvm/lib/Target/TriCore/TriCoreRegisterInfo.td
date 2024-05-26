//===- TriCoreRegisterInfo.td - TriCore Register defs ------------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//  Declarations that describe the TriCore register file
//===----------------------------------------------------------------------===//

// Registers are identified with 5-bit ID numbers.
class TriCoreReg<bits<5> Enc, string n> : Register<n> {
  let HWEncoding{15-5} = 0;
  let HWEncoding{4-0} = Enc;
  let Namespace = "TRICORE";
}

let Namespace = "TRICORE" in {
  def sub_32 : SubRegIndex<32>;
}

// Integer registers
foreach i = 0-31 in {
  def R#i : TriCoreReg<i, "R"#i>, DwarfRegNum<[i]>;
}

// Register classes.
class TriCoreRegisterClass<list<ValueType> types, int size,
                          int alignment, dag regList, int copyCost = 1>
    : RegisterClass <"TRICORE", types, alignment, regList> {
      let Size = size;
      let CopyCost = copyCost;
}

def GPR : TriCoreRegisterClass <[i32], 32, 32, (add (sequence "R%u", 0, 31))>;

// Register operands
class TriCoreRegisterOperand<RegisterClass RC> : RegisterOperand<RC>
{
  let DecoderMethod = "decode"#RC#"RegisterClass";
}

def GPROperand : TriCoreRegisterOperand<GPR>
{
  // Add specifics for certain registers here ...
  // Ex: If R0 is a constant register and always returns 0, we would specify this as following
  // let GIZeroRegister = R0;
}
