# Configuration library for UI element and style configurators

add_library(configuration INTERFACE)

target_include_directories(configuration
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(configuration
  INTERFACE
  types
  interfaces
  events
)
