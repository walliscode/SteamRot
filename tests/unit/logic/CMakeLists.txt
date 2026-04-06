add_executable(test_logic
LogicFactory.test.cpp
ui_collision.test.cpp
action_ui.test.cpp
ui_render.test.cpp
logic_render_test_helpers.cpp
ui_helpers.test.cpp
TestLogic.cpp
action_grimoire_machina.test.cpp

render_grimoire_machina.test.cpp
positioning_grimoire_machina.test.cpp
collision_mouse.test.cpp
pixel_render.test.cpp

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
  matchers
  context_test_helpers
  harness


)

catch_discover_tests(test_logic)

