add_library(test_data_generators
  test_data_generator.cpp
)

target_include_directories(test_data_generators
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_data_generators
  PUBLIC
  test_context
  Catch2::Catch2
)

# Create test executable for data generators
add_executable(test_data_generators_tests
  test_data_generator.test.cpp
  examples.test.cpp
)

target_link_libraries(test_data_generators_tests
  PRIVATE
  test_data_generators
  entity
  Catch2::Catch2WithMain
)

catch_discover_tests(test_data_generators_tests
  TEST_PREFIX "data_generators."
  PROPERTIES LABELS "unit"
)
