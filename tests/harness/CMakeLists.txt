add_library(test_harness
  test_data_harness.cpp
  TestFixture.cpp
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
  entity
  test_matchers
  resources
  scenes
  logic
  Catch2::Catch2
)

# Create test executable for harness
add_executable(test_harness_tests
  test_data_harness.test.cpp
  simulation_runner.test.cpp
  input_simulation.test.cpp
  event_simulation.test.cpp
)

target_link_libraries(test_harness_tests
  PRIVATE
  test_harness
  Catch2::Catch2WithMain
)

catch_discover_tests(test_harness_tests
  TEST_PREFIX "harness."
  PROPERTIES LABELS "unit"
)
