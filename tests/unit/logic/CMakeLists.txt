add_executable(test_logic
LogicFactory.test.cpp
draw_ui_elements.test.cpp
draw_ui_elements_test_helpers.cpp
collision.test.cpp
ui_helpers.test.cpp
logic_execution.test.cpp
)

target_include_directories(test_logic
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_logic
  PRIVATE
  Catch2::Catch2WithMain
  logic
  test_logic_helpers
  context_test_helpers
  test_harness
  reporters_objects
)

catch_discover_tests(test_logic)

add_executable(dd_logic_tests
  logic_data_driven.test.cpp
)

target_include_directories(dd_logic_tests
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dd_logic_tests
  PRIVATE
  Catch2::Catch2WithMain
  logic
  test_logic_helpers
  context_test_helpers
  test_harness
  reporters_objects
)

catch_discover_tests(dd_logic_tests
  TEST_PREFIX "logic"
  REPORTER "harness"
)
add_library(test_logic_helpers
  draw_ui_elements_test_helpers.cpp
  logic_test_helpers.cpp

)

target_include_directories(test_logic_helpers
PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_logic_helpers
  PUBLIC
  Catch2::Catch2WithMain
  logic

)
