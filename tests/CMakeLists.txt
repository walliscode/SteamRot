add_executable(steamrot_tests


  unit_tests/directory_paths.test.cpp

  unit_tests/error_log.test.cpp
  unit_tests/info_log.test.cpp

  )

target_link_libraries(steamrot_tests
  PRIVATE
  Catch2::Catch2WithMain
  systems
  steamrot_directory_paths)

target_include_directories(steamrot_tests
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/test_utils)
message (STATUS "Tests enabled")


list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(steamrot_tests)
