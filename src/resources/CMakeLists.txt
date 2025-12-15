add_library(resources
  INTERFACE
)
target_include_directories(resources
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(resources
  INTERFACE
  types
  assets
  events
)


