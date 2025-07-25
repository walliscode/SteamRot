add_executable(test_data_handlers
  FlatbuffersDataLoader.test.cpp
)

target_link_libraries(test_data_handlers
PRIVATE
  Catch2::Catch2WithMain
  data_handlers
)

catch_discover_tests(test_data_handlers)
