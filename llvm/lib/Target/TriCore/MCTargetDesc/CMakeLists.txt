add_llvm_component_library(LLVMTriCoreDesc
  TriCoreAsmBackend.cpp
  TriCoreELFObjectWriter.cpp
  TriCoreInstPrinter.cpp
  TriCoreMCAsmInfo.cpp
  TriCoreMCCodeEmitter.cpp
  TriCoreMCExpr.cpp
  TriCoreMCTargetDesc.cpp

  LINK_COMPONENTS
  TriCoreInfo
  MC
  MCDisassembler
  Support
  TargetParser

  ADD_TO_COMPONENT
  TriCore
)
