add_library(test_context
  TestContext.cpp
  TestScenarios.cpp
  test_assertions.cpp)

target_include_directories(test_context
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_context
  PUBLIC
  context
  Catch2::Catch2
)

