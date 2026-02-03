add_executable(test_logic
LogicFactory.test.cpp
logic_collision.test.cpp
logic_action.test.cpp
logic_render.test.cpp
logic_render_test_helpers.cpp
logic_ui.test.cpp

)

target_include_directories(test_logic
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_logic
  PRIVATE
  Catch2::Catch2WithMain
  logic
  context
  resources
  entity
  context_test_helpers


)

catch_discover_tests(test_logic)

