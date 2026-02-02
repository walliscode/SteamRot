add_executable(test_harness
  FlatbuffersTestDataLoader.test.cpp
  FlatbuffersTestDataProvider.test.cpp
  TestEngine.test.cpp
  harness_runner.test.cpp
  RunHarnessTests.test.cpp
  SimulationRunner.test.cpp
  add_uuids.test.cpp
)

target_link_libraries(test_harness
  PRIVATE
  Catch2::Catch2WithMain
  harness
  matchers
  context_test_helpers
)

catch_discover_tests(test_harness)



