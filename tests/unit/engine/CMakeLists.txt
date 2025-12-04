add_executable(test_engine
Engine.test.cpp
GameEngine.test.cpp
)

target_include_directories(test_engine
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_engine
  PRIVATE
  Catch2::Catch2WithMain
  engine
  context_test_helpers
  test_harness


)

catch_discover_tests(test_engine)
