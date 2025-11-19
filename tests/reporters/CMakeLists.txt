# reporters library
# providing custom output formats for test results


add_library(reporters_objects OBJECT
  HarnessReporter.cpp
)

target_link_libraries(reporters_objects
  PUBLIC
    Catch2::Catch2
    conmat::conmat
)

target_include_directories(reporters_objects
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(reporter_test
  HarnessReporter.test.cpp
)


target_link_libraries(reporter_test
  PUBLIC
    Catch2::Catch2WithMain
    reporters_objects
)


catch_discover_tests(reporter_test
  TEST_PREFIX
    "reporters."
  REPORTER
    "harness"

)
