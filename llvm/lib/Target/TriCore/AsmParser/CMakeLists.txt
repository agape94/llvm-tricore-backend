include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_component_library(LLVMTriCoreAsmParser
  TriCoreAsmParser.cpp

  LINK_COMPONENTS
  CodeGenTypes
  TriCoreDesc
  TriCoreInfo
  MC
  MCParser
  Support

  ADD_TO_COMPONENT
  TriCore
  )

add_dependencies( LLVMTriCoreAsmParser TriCoreCommonTableGen )
