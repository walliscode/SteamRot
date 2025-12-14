add_library(interfaces
INTERFACE
)

target_include_directories(interfaces
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(interfaces
  INTERFACE
  types
)

