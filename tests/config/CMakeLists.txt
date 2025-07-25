
configure_file(
steamrot_directory_paths.test.cpp.in
steamrot_directory_paths.test.cpp
@ONLY)


# test the non testing version of the config
add_executable(test_config_without_testing
steamrot_directory_paths.test.cpp
)

target_link_libraries(test_config_without_testing
PRIVATE
  Catch2::Catch2WithMain
steamrot_directory_paths
)

catch_discover_tests(test_config_without_testing)

# test the testing version of the config
add_executable(test_config_with_testing
steamrot_directory_paths.test.cpp
)

target_compile_definitions(test_config_with_testing
PRIVATE
USE_TEST_DIRECTORIES=1
)

target_link_libraries(test_config_with_testing
PRIVATE
  Catch2::Catch2WithMain
  steamrot_directory_paths
)
catch_discover_tests(test_config_with_testing)
