# Execution module for test harness
# Provides level-based executors for testing at different granularities

add_library(execution
  execution.cpp
  logic_executor.cpp
  scene_executor.cpp
  scene_manager_executor.cpp
  game_loop_executor.cpp
  execution_runner.cpp
)

target_include_directories(execution
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  # Include parent directory for TestFixture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(execution
  PUBLIC
  logic
  events
  context
  entity
  scenes
  systems
  flatbuffers_headers
)
