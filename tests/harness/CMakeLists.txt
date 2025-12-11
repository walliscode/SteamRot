add_library(test_harness
  test_harness.cpp
  test_data_loader.cpp
  test_data_comparison.cpp
  TestEngine.cpp
  simulation_runner.cpp
  input_simulation.cpp
  event_simulation.cpp

)

target_include_directories(test_harness
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_harness
  PUBLIC
  context
  components
  entity
  matchers
  scenes
  logic
  engine
  events
  Catch2::Catch2
  conmat
)

