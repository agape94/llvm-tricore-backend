//===-- TriCoreTargetTransformInfo.h - TriCore specific TTI ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file a TargetTransformInfo::Concept conforming object specific to the
// TriCore target machine. It uses the target's detailed information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TRICORE_TRICORETARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_TRICORE_TRICORETARGETTRANSFORMINFO_H

#include "TriCore.h"
#include "TriCoreSubtarget.h"
#include "TriCoreTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/Support/MathExtras.h"

namespace llvm {
class TriCoreTTIImpl : public BasicTTIImplBase<TriCoreTTIImpl> {
  typedef BasicTTIImplBase<TriCoreTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const TriCoreSubtarget *ST;
  const TriCoreTargetLowering *TLI;

  const TriCoreSubtarget *getST() const { return ST; }
  const TriCoreTargetLowering *getTLI() const { return TLI; }

public:
  explicit TriCoreTTIImpl(const TriCoreTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_TRICORE_TRICORETARGETTRANSFORMINFO_H
