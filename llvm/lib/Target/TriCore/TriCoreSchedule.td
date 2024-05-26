//=-TriCoreSchedule.td - TriCore Scheduling Definitions --*- tablegen -*-=========//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

def TriCoreItinerary : ProcessorItineraries<[],[],[]>;

def TriCoreSchedModel : SchedMachineModel {
  // Cycles for loads to access the cache [default = -1]
  let LoadLatency = 2;

  // Max micro-ops that can be buffered for optimized loop dispatch/execution.
  // [default = -1]
  let LoopMicroOpBufferSize = 0;

  // Allow scheduler to assign default model to any unrecognized opcodes.
  // [default = 1]
  let CompleteModel = 0;

  // Max micro-ops that may be scheduled per cycle. [default = 1]
  let IssueWidth = 1;

  // Extra cycles for a mispredicted branch. [default = -1]
  let MispredictPenalty = 10;

  // Enable Post RegAlloc Scheduler pass. [default = 0]
  let PostRAScheduler = 0;

  // Max micro-ops that can be buffered. [default = -1]
  let MicroOpBufferSize = 0;

  // Per-cycle resources tables. [default = NoItineraries]
  let Itineraries = TriCoreItinerary;
}
