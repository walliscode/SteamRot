#################################################
# DataDrivenTests.cmake
#
# CMake functions for creating data-driven test executables.
#
# Usage:
#   add_data_driven_test(
#     NAME logic_collision
#     DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data
#     TAG collision
#   )
#################################################

#################################################
# add_data_driven_test
#
# Creates a test executable that runs all test data configs from a directory.
#
# Arguments:
#   NAME      - Name of the test (used for executable name)
#   DATA_DIR  - Path to directory containing .test_data.bin files
#   TAG       - Catch2 tag for the test
#################################################
function(add_data_driven_test)
  # Parse arguments
  cmake_parse_arguments(
    ARG                           # Prefix
    ""                            # Options
    "NAME;DATA_DIR;TAG"           # One-value keywords
    ""                            # Multi-value keywords
    ${ARGN}
  )

  # Validate required arguments
  if(NOT ARG_NAME)
    message(FATAL_ERROR "add_data_driven_test: NAME is required")
  endif()

  if(NOT ARG_DATA_DIR)
    message(FATAL_ERROR "add_data_driven_test: DATA_DIR is required")
  endif()

  if(NOT ARG_TAG)
    set(ARG_TAG ${ARG_NAME})
  endif()

  # Set variables for template substitution
  set(TEST_NAME ${ARG_NAME})
  set(TEST_DIR_NAME ${ARG_NAME})
  set(DATA_DIRECTORY ${ARG_DATA_DIR})
  set(TEST_TAG ${ARG_TAG})

  # Configure the test source file from template
  configure_file(
    ${CMAKE_SOURCE_DIR}/tests/cmake/data_driven_test.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_data_driven_test.cpp
    @ONLY
  )

  # Create the test executable
  add_executable(test_${ARG_NAME}_data
    ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_data_driven_test.cpp
  )

  # Link with required libraries
  target_link_libraries(test_${ARG_NAME}_data
    PRIVATE
    test_harness
    Catch2::Catch2WithMain
  )

  # Discover tests
  catch_discover_tests(test_${ARG_NAME}_data
    TEST_PREFIX "${ARG_NAME}_data."
    REPORTER "harness"
  )
endfunction()

#################################################
# add_data_driven_test_with_custom
#
# Creates a test that combines data-driven tests with custom test cases.
#
# Arguments:
#   NAME       - Name of the test
#   DATA_DIR   - Path to directory containing .test_data.bin files
#   TAG        - Catch2 tag for the test
#   SOURCES    - Additional source files with custom tests
#################################################
function(add_data_driven_test_with_custom)
  cmake_parse_arguments(
    ARG
    ""
    "NAME;DATA_DIR;TAG"
    "SOURCES"
    ${ARGN}
  )

  if(NOT ARG_NAME)
    message(FATAL_ERROR "add_data_driven_test_with_custom: NAME is required")
  endif()

  if(NOT ARG_DATA_DIR)
    message(FATAL_ERROR "add_data_driven_test_with_custom: DATA_DIR is required")
  endif()

  if(NOT ARG_TAG)
    set(ARG_TAG ${ARG_NAME})
  endif()

  # Set variables for template substitution
  set(TEST_NAME ${ARG_NAME})
  set(TEST_DIR_NAME ${ARG_NAME})
  set(DATA_DIRECTORY ${ARG_DATA_DIR})
  set(TEST_TAG ${ARG_TAG})

  # Configure the data-driven test source
  configure_file(
    ${CMAKE_SOURCE_DIR}/tests/cmake/data_driven_test.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_data_driven_test.cpp
    @ONLY
  )

  # Create executable with both generated and custom sources
  add_executable(test_${ARG_NAME}_combined
    ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}_data_driven_test.cpp
    ${ARG_SOURCES}
  )

  target_link_libraries(test_${ARG_NAME}_combined
    PRIVATE
    test_harness
    Catch2::Catch2WithMain
  )

  catch_discover_tests(test_${ARG_NAME}_combined
    TEST_PREFIX "${ARG_NAME}."
    REPORTER "harness"
  )
endfunction()
