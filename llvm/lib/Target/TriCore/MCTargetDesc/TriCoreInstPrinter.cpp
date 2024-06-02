//===-- TriCoreInstPrinter.cpp - Convert TriCore MCInst to asm syntax ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an TriCore MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "TriCoreInstPrinter.h"
#include "TriCoreMCExpr.h"
#include "TriCoreAluCode.h"
#include "MCTargetDesc/TriCoreMCTargetDesc.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "TriCoreGenAsmWriter.inc"

void TriCoreInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &OS, const char *Modifier) {
  assert((Modifier == nullptr || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg())
    OS << "%" << getRegisterName(Op.getReg());
  else if (Op.isImm())
    OS << formatHex(Op.getImm());
  else {
    assert(Op.isExpr() && "Expected an expression");
    Op.getExpr()->print(OS, &MAI);
  }
}

void TriCoreInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                 StringRef Annotation,
                                 const MCSubtargetInfo & /*STI*/,
                                 raw_ostream &OS) {

  printInstruction(MI, Address, OS);
}

//===----------------------------------------------------------------------===//
// PrintSExtImm<unsigned bits>
//===----------------------------------------------------------------------===//
template <unsigned bits>
void TriCoreInstPrinter::printSExtImm(const MCInst *MI, unsigned OpNo,
                                       raw_ostream &O) {
  if (MI->getOperand(OpNo).isImm()) {
    int64_t Value = MI->getOperand(OpNo).getImm();
    Value = SignExtend32<bits>(Value);
    assert(isInt<bits>(Value) && "Invalid simm argument");
    O << Value;
  }
  else
    printOperand(MI, OpNo, O);
}

template <unsigned bits>
void TriCoreInstPrinter::printZExtImm(const MCInst *MI, int OpNo,
                                       raw_ostream &O) {
  if (MI->getOperand(OpNo).isImm()) {
    unsigned int Value = MI->getOperand(OpNo).getImm();
    assert(Value <= ((unsigned int)pow(2,bits) -1 )  && "Invalid uimm argument!");
    O << (unsigned int)Value;
  }
  else
    printOperand(MI, OpNo, O);
}

// Print a 'bo' operand which is an addressing mode
// Base+Offset
void TriCoreInstPrinter::printAddrBO(const MCInst *MI, unsigned OpNum,
                                         raw_ostream &O) {

  const MCOperand &Base = MI->getOperand(OpNum);
  const MCOperand &Offset = MI->getOperand(OpNum+1);

  unsigned Opcode = MI->getOpcode();

  switch (Opcode) {
    default:
      // Print register base field
      if (Base.isReg())
          O << "[%" << StringRef(getRegisterName(Base.getReg())).lower() << "]";

      if (Offset.isExpr())
        Offset.getExpr()->print(O, &MAI);
      else {
        assert(Offset.isImm() && "Expected immediate in displacement field");
        O << " " << Offset.getImm();
      }
      break;
    }
}

// Print a 'preincbo' operand which is an addressing mode
// Pre-increment Base+Offset
void TriCoreInstPrinter::printAddrPreIncBO(const MCInst *MI, unsigned OpNum,
                                         raw_ostream &O) {

  const MCOperand &Base = MI->getOperand(OpNum);
  const MCOperand &Offset = MI->getOperand(OpNum+1);

  // Print register base field
  if (Base.isReg())
      O << "[+%" << StringRef(getRegisterName(Base.getReg())).lower() << "]";

  if (Offset.isExpr())
    Offset.getExpr()->print(O, &MAI);
  else {
    assert(Offset.isImm() && "Expected immediate in displacement field");
    O << " " << Offset.getImm();
  }
}

// Print a 'postincbo' operand which is an addressing mode
// Post-increment Base+Offset
void TriCoreInstPrinter::printAddrPostIncBO(const MCInst *MI, unsigned OpNum,
                                         raw_ostream &O) {

  const MCOperand &Base = MI->getOperand(OpNum);
  const MCOperand &Offset = MI->getOperand(OpNum+1);

  // Print register base field
  if (Base.isReg())
      O << "[%" << StringRef(getRegisterName(Base.getReg())).lower() << "+]";

  if (Offset.isExpr())
    Offset.getExpr()->print(O, &MAI);
  else {
    assert(Offset.isImm() && "Expected immediate in displacement field");
    O << " " << Offset.getImm();
  }
}

// Print a 'circbo' operand which is an addressing mode
// Circular Base+Offset
void TriCoreInstPrinter::printAddrCircBO(const MCInst *MI, unsigned OpNum,
                                         raw_ostream &O) {

  const MCOperand &Base = MI->getOperand(OpNum);
  const MCOperand &Offset = MI->getOperand(OpNum+1);

  // Print register base field
  if (Base.isReg()) {
      O << "[";
      printRegName(O, MRI.getSubReg(Base.getReg(), TRICORE::subreg_even));
      O << "/";
      printRegName(O, MRI.getSubReg(Base.getReg(), TRICORE::subreg_odd));
      O << "+c]";
  }
  if (Offset.isExpr())
    Offset.getExpr()->print(O, &MAI);
  else {
    assert(Offset.isImm() && "Expected immediate in displacement field");
    O << " " << Offset.getImm();
  }
}

// Print a 'bitrevbo' operand which is an addressing mode
// Bit-Reverse Base+Offset
void TriCoreInstPrinter::printAddrBitRevBO(const MCInst *MI, unsigned OpNum,
                                         raw_ostream &O) {

  const MCOperand &Base = MI->getOperand(OpNum);

  // Print register base field
  if (Base.isReg()) {
      O << "[";
      printRegName(O, MRI.getSubReg(Base.getReg(), TRICORE::subreg_even));
      O << "/";
      printRegName(O, MRI.getSubReg(Base.getReg(), TRICORE::subreg_odd));
      O << "+r]";
  }
}

void TriCoreInstPrinter::printPairAddrRegsOperand(const MCInst *MI, unsigned OpNo,
                                             raw_ostream &O) {
  unsigned AddrReg = MI->getOperand(OpNo).getReg();

  O << "[";
  printRegName(O, MRI.getSubReg(AddrReg, TRICORE::subreg_even));
  O << "/";
  printRegName(O, MRI.getSubReg(AddrReg, TRICORE::subreg_odd));
  O << "]";
}

void TriCoreInstPrinter::printPCRelImmOperand(const MCInst *MI, unsigned OpNo,
                                             raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isImm())
    O << Op.getImm();
  else {
    assert(Op.isExpr() && "unknown pcrel immediate operand");
    Op.getExpr()->print(O, &MAI);
  }
}
