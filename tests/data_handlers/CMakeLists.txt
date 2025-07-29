configure_file(
  PathProvider.test.cpp.in
  PathProvider.test.cpp
  @ONLY)


add_executable(test_data_handlers
  PathProvider.test.cpp
  FlatbuffersDataLoader.test.cpp
)

target_compile_definitions(test_data_handlers
PRIVATE
USE_TEST_DIRECTORIES)

target_link_libraries(test_data_handlers
PRIVATE
  Catch2::Catch2WithMain
  data_handlers
  components
)

catch_discover_tests(test_data_handlers)
