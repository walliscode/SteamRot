add_executable(test_display
DisplayManager.test.cpp
)

target_include_directories(test_display
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_display
  PRIVATE
  Catch2::Catch2WithMain
  display
  context_test_helpers
)

catch_discover_tests(test_display)
