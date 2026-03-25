add_library(harness
  TestEngine.cpp
  harness_runner.cpp
  SimulationRunner.cpp
  add_uuids.cpp
)

target_include_directories(harness
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(harness
  PUBLIC
  engine
  types
  matchers
)

