add_llvm_component_library(LLVMTriCoreInfo
  TriCoreTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  TriCore
  )
