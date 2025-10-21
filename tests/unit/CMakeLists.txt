# Unit tests directory
# Unit tests test individual classes/functions in isolation with mocked dependencies
# Run with: ctest -L unit

add_executable(test_data_system
  test_data_system.test.cpp
)

target_link_libraries(test_data_system
  PRIVATE
  test_context
  Catch2::Catch2WithMain
)

add_test(NAME test_data_system COMMAND test_data_system)
set_tests_properties(test_data_system PROPERTIES LABELS "unit")
