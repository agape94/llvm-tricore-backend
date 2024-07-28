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
#include <cmath>
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
#include "llvm/Support/Casting.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

// Include the auto-generated portion of the assembly writer.
#define PRINT_ALIAS_INSTR
#include "TriCoreGenAsmWriter.inc"

static void printExpr(const MCExpr *Expr, const MCAsmInfo *MAI,
                      raw_ostream &OS) {
  int Offset = 0;
  const MCSymbolRefExpr *SRE;

  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
    SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());
    assert(SRE && CE && "Binary expression must be sym+const.");
    Offset = CE->getValue();
  } else {
    SRE = dyn_cast<MCSymbolRefExpr>(Expr);
    assert(SRE && "Unexpected MCExpr type.");
  }

  MCSymbolRefExpr::VariantKind Kind = SRE->getKind();

  switch (Kind) {
    default:                                 llvm_unreachable("Invalid kind!");
    case MCSymbolRefExpr::VK_None:           break;
    case MCSymbolRefExpr::VK_TRICORE_HI_OFFSET:    OS << "hi:";     break;
    case MCSymbolRefExpr::VK_TRICORE_LO_OFFSET:   
    {
      OS << "lo:";
      OS << SRE->getSymbol();
      if (Offset) {
        if (Offset > 0) {
          OS << '+';
        }
        OS << Offset;
      }
      break;
    }
      
  }

  SRE->getSymbol().print(OS, MAI);

  if (Offset) {
    if (Offset > 0)
      OS << '+';
    OS << Offset;
  }
}

void TriCoreInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                    raw_ostream &OS, const char *Modifier) {
  // assert((Modifier == nullptr || Modifier[0] == 0) && "No modifiers supported");
  // const MCOperand &Op = MI->getOperand(OpNo);
  // if (Op.isReg())
  //   OS << "%" << getRegisterName(Op.getReg());
  // else if (Op.isImm())
  //   OS << formatHex(Op.getImm());
  // else {
  //   assert(Op.isExpr() && "Expected an expression");
  //   Op.getExpr()->print(OS, &MAI);
  // }
  assert((Modifier == nullptr || Modifier[0] == 0) && "No modifiers supported");
  const MCOperand &Op = MI->getOperand(OpNo);

  if (Op.isReg()) {
    printRegName(OS, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    OS << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  printExpr(Op.getExpr(), &MAI, OS);
}

void TriCoreInstPrinter::printCCOperand(const MCInst *MI, unsigned OpNo,
                                       raw_ostream &OS) {
  unsigned CC = MI->getOperand(OpNo).getImm();

  switch (CC) {
  default:
   llvm_unreachable("Unsupported CC code");
  case 0:
   OS << "eq";
   break;
  case 1:
   OS << "ne";
   break;
  case 3:
   OS << "lt";
   break;
  case 2:
   OS << "ge";
   break;
  }
}

void TriCoreInstPrinter::printRegName(raw_ostream &OS, MCRegister Reg) const {
  OS << "%" << StringRef(getRegisterName(Reg)).lower();
}

void TriCoreInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                 StringRef Annotation,
                                 const MCSubtargetInfo & /*STI*/,
                                 raw_ostream &OS) {

  unsigned Opcode = MI->getOpcode();

  switch (Opcode) {
    // Combine 2 AddrRegs from disassember into a PairAddrRegs to match
    // with instr def. load/store require even/odd AddrReg pair. To enforce
    // this constraint, a single PairAddrRegs reg operand is used in the .td
    // file to replace the two AddrRegs. However, when decoding them, the two
    // AddrRegs cannot be automatically expressed as a PairAddrRegs, so we
    // have to manually merge them.
    // FIXME: We would really like to be able to tablegen'erate this.
    case TRICORE::LD_DAabs:
    case TRICORE::LD_DAbo:
    case TRICORE::LD_DApreincbo:
    case TRICORE::LD_DApostincbo:
    case TRICORE::ST_Bcircbo:
    case TRICORE::ST_Hcircbo:
    case TRICORE::ST_Wcircbo:
    case TRICORE::ST_Dcircbo:
    case TRICORE::ST_Qcircbo:
    case TRICORE::ST_Acircbo:
    case TRICORE::ST_Bbitrevbo:
    case TRICORE::ST_Hbitrevbo:
    case TRICORE::ST_Wbitrevbo:
    case TRICORE::ST_Dbitrevbo:
    case TRICORE::ST_Qbitrevbo:
    case TRICORE::ST_Abitrevbo: {
      const MCRegisterClass &MRC = MRI.getRegClass(TRICORE::AddrRegsRegClassID);
      unsigned Reg = MI->getOperand(0).getReg();
      if (MRC.contains(Reg)) {
        MCInst NewMI;
        MCOperand NewReg;
        NewMI.setOpcode(Opcode);

        NewReg = MCOperand::createReg(MRI.getMatchingSuperReg(
            Reg, TRICORE::subreg_even,
            &MRI.getRegClass(TRICORE::PairAddrRegsRegClassID)));
        NewMI.addOperand(NewReg);

        // Copy the rest operands into NewMI.
        for (unsigned i = 2; i < MI->getNumOperands(); ++i)
          NewMI.addOperand(MI->getOperand(i));
        printInstruction(&NewMI, Address, OS);
        return;
      }
      break;
    }
    case TRICORE::ST_DAabs:
    case TRICORE::ST_DAbo:
    case TRICORE::ST_DApreincbo:
    case TRICORE::ST_DApostincbo:
    case TRICORE::LD_Bcircbo:
    case TRICORE::LD_BUcircbo:
    case TRICORE::LD_Hcircbo:
    case TRICORE::LD_HUcircbo:
    case TRICORE::LD_Wcircbo:
    case TRICORE::LD_Dcircbo:
    case TRICORE::LD_Acircbo:
    case TRICORE::LD_Bbitrevbo:
    case TRICORE::LD_BUbitrevbo:
    case TRICORE::LD_Hbitrevbo:
    case TRICORE::LD_HUbitrevbo:
    case TRICORE::LD_Wbitrevbo:
    case TRICORE::LD_Dbitrevbo:
    case TRICORE::LD_Abitrevbo: {
      const MCRegisterClass &MRC = MRI.getRegClass(TRICORE::AddrRegsRegClassID);
      unsigned Reg = MI->getOperand(1).getReg();      
      if (MRC.contains(Reg)) {
        MCInst NewMI;
        MCOperand NewReg;
        NewMI.setOpcode(Opcode);
        
        NewMI.addOperand(MI->getOperand(0));
        NewReg = MCOperand::createReg(MRI.getMatchingSuperReg(
            Reg, TRICORE::subreg_even,
            &MRI.getRegClass(TRICORE::PairAddrRegsRegClassID)));
        NewMI.addOperand(NewReg);

        // Copy the rest operands into NewMI.
        for (unsigned i = 3; i < MI->getNumOperands(); ++i)
          NewMI.addOperand(MI->getOperand(i));
        printInstruction(&NewMI, Address, OS);
        return;
      }
      break;
    }    
    case TRICORE::LD_DAcircbo:
    case TRICORE::ST_DAcircbo:
    case TRICORE::LD_DAbitrevbo:
    case TRICORE::ST_DAbitrevbo: {
      const MCRegisterClass &MRC = MRI.getRegClass(TRICORE::AddrRegsRegClassID);
      unsigned Reg1 = MI->getOperand(0).getReg();
      unsigned Reg2 = MI->getOperand(2).getReg();
      if (MRC.contains(Reg2)) {
        MCInst NewMI;
        NewMI.setOpcode(Opcode);
        
        NewMI.addOperand(MCOperand::createReg(MRI.getMatchingSuperReg(
          Reg1, TRICORE::subreg_even,
          &MRI.getRegClass(TRICORE::PairAddrRegsRegClassID))));
       
        NewMI.addOperand(MCOperand::createReg(MRI.getMatchingSuperReg(
          Reg2, TRICORE::subreg_even,
          &MRI.getRegClass(TRICORE::PairAddrRegsRegClassID))));

        // Copy the rest operands into NewMI.
        for (unsigned i = 4; i < MI->getNumOperands(); ++i)
          NewMI.addOperand(MI->getOperand(i));
        printInstruction(&NewMI, Address, OS);
        return;
      }
      break;
    }
  }

  printInstruction(MI, Address, OS);
  printAnnotation(OS, Annotation);
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
