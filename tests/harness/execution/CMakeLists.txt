# Execution module for test harness
# Provides level-based executors for testing at different granularities

add_library(execution
  execution.cpp
  # Additional files will be added as they are created in future stages
)

target_include_directories(execution
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(execution
  PUBLIC
  logic
  events
  context
  entity
  flatbuffers_headers
)
