add_executable(test_logic
LogicFactory.test.cpp
draw_ui_elements.test.cpp
draw_ui_elements_test_helpers.cpp
collision.test.cpp
ui_helpers.test.cpp
UICollisionLogic.test.cpp
UIActionLogic.test.cpp
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
)

catch_discover_tests(test_logic)

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
