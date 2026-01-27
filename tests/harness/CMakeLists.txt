add_library(harness
  TestEngine.cpp
  FlatbuffersTestDataLoader.cpp
  FlatbuffersTestDataProvider.cpp
  harness_runner.cpp
  SimulationRunner.cpp
  UUIDAssignmentTracker.cpp
  configure_test_engine_snapshot.cpp
)

target_include_directories(harness
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(harness
  PUBLIC
  engine
  types
  data_providers
)

