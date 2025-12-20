
# Compile entity test data
include(compile_test_data.cmake)

add_executable(test_user_interface
  FlatbuffersUIElementConfigurator.test.cpp
)

target_link_libraries(test_user_interface
  PRIVATE
  user_interface
  types
  configuration
  Catch2::Catch2WithMain
  context_test_helpers
)

# Ensure test data is compiled before building tests
add_dependencies(test_user_interface compile_ui_element_unit_test_data  )

catch_discover_tests(test_user_interface)
