add_executable(test_systems
GameEngine.test.cpp
game_engine_config.test.cpp
)

target_include_directories(test_systems
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_systems
  PRIVATE
  Catch2::Catch2WithMain
  systems
  context_test_helpers
  test_harness


)

catch_discover_tests(test_systems)
