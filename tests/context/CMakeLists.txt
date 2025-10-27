add_library(context_test_helpers
  TestContext.cpp
  TestScenarios.cpp
  test_assertions.cpp
  TestFixture.cpp)

target_include_directories(context_test_helpers
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(context_test_helpers
  PUBLIC
  context
  resources
  scenes
  Catch2::Catch2
)

