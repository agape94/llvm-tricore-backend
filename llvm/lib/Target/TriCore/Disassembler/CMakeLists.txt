add_llvm_component_library(LLVMTriCoreDisassembler
  TriCoreDisassembler.cpp

  LINK_COMPONENTS
  CodeGenTypes
  TriCoreDesc
  TriCoreInfo
  MC
  MCDisassembler
  Support

  ADD_TO_COMPONENT
  TriCore
  )
