add_executable(test_systems
GameEngine.test.cpp
)

target_include_directories(test_systems
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_systems
  PRIVATE
  Catch2::Catch2WithMain
  systems

)

catch_discover_tests(test_systems)
