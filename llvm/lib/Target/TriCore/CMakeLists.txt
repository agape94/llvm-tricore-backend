add_llvm_component_group(TriCore)

set(LLVM_TARGET_DEFINITIONS TriCore.td)

tablegen(LLVM TriCoreGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM TriCoreGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM TriCoreGenCallingConv.inc -gen-callingconv)
tablegen(LLVM TriCoreGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TriCoreGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM TriCoreGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TriCoreGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM TriCoreGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TriCoreGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(TriCoreCommonTableGen)

add_llvm_target(TriCoreCodeGen
  TriCoreAsmPrinter.cpp
  TriCoreFrameLowering.cpp
  TriCoreInstrInfo.cpp
  TriCoreISelDAGToDAG.cpp
  TriCoreISelLowering.cpp
  TriCoreMachineFunctionInfo.cpp
  TriCoreMCInstLower.cpp
  TriCoreRegisterInfo.cpp
  TriCoreSelectionDAGInfo.cpp
  TriCoreSubtarget.cpp
  TriCoreTargetMachine.cpp
  TriCoreTargetObjectFile.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  TriCoreAsmParser
  TriCoreDesc
  TriCoreInfo
  MC
  SelectionDAG
  Support
  Target
  TargetParser
  TransformUtils

  ADD_TO_COMPONENT
  TriCore
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
