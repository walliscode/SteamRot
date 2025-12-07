# Core Types Interface Library
# Header-only library containing fundamental data structures
# No external dependencies except standard library

add_library(core INTERFACE)

target_include_directories(core
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Core has no dependencies - it's at the bottom of the dependency tree
# This enables separation of data structures from implementations
