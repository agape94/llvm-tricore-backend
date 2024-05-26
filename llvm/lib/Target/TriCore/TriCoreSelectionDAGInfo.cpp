//===-- TriCoreSelectionDAGInfo.cpp - TriCore SelectionDAG Info -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the TriCoreSelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#include "TriCoreSelectionDAGInfo.h"

#include "TriCoreTargetMachine.h"

#define DEBUG_TYPE "tricore-selectiondag-info"

namespace llvm {

SDValue TriCoreSelectionDAGInfo::EmitTargetCodeForMemcpy(
    SelectionDAG & /*DAG*/, const SDLoc & /*dl*/, SDValue /*Chain*/,
    SDValue /*Dst*/, SDValue /*Src*/, SDValue Size, Align /*Alignment*/,
    bool /*isVolatile*/, bool /*AlwaysInline*/,
    MachinePointerInfo /*DstPtrInfo*/,
    MachinePointerInfo /*SrcPtrInfo*/) const {
  return SDValue();
}

} // namespace llvm
