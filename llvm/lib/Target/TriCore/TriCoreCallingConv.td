//===- TriCoreCallingConv.td - Calling Conventions TriCore -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the TriCore architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Return Value Calling Conventions
//===----------------------------------------------------------------------===//

// TriCore C Calling convention.
def CC_TriCore : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i8, i16], CCPromoteToType<i32>>,

	
	// The first 4 integer arguments are passed in integer registers.
	//	CCIfType<[i32], CCAssignToReg<[D4, D5, D6, D7]>>,
	
	//CCIfType<[i32], CCAssignToReg<[A4, A5, A6, A7]>>,
	
	// Pointer arguments are handled inside TriCoreIselLowering, because
	// LLVM lowers i32** type into i32, hence there is no way to distingusish
	// beetwen a pointer type and an integer type.
	

	
	
  //CCDelegateTo<CC_TriCore_StackModel>
  
  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;
// TriCore C return-value convention.
def RetCC_TriCore : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i8, i16], CCPromoteToType<i32>>,
  
  // i32 are returned in registers D2
  CCIfType<[i32], CCAssignToReg<[D2]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;
