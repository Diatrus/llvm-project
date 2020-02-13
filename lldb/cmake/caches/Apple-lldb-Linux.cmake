include(${CMAKE_CURRENT_LIST_DIR}/Apple-lldb-base.cmake)

set(LLVM_DISTRIBUTION_COMPONENTS
  lldb
  liblldb
  lldb-argdumper
  lldb-server
  repl_swift
  # SWIFT_ENABLE_TENSORFLOW
  lldb-python-scripts
  CACHE STRING "")
