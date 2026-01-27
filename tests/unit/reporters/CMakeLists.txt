# Unit tests for reporters library

add_executable(test_reporters
  HarnessReporter.test.cpp
)

target_link_libraries(test_reporters
  PUBLIC
    Catch2::Catch2WithMain
    reporters_objects
)

catch_discover_tests(test_reporters
  TEST_PREFIX
    "unit.reporters."
  REPORTER
    "harness"
)
