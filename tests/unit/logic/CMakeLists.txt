add_executable(test_logic
LogicFactory.test.cpp
# Logic.test.cpp
action_ui.test.cpp
action_ghost.test.cpp
action_camera.test.cpp
positioning_camera.test.cpp
render_ui.test.cpp
logic_render_test_helpers.cpp
grimoire_machina_test_helpers.cpp
state_ui.test.cpp
TestLogic.cpp
action_grimoire_machina.test.cpp
positioning_grimoire_machina.test.cpp
positioning_ghost.test.cpp
render_grimoire_machina.test.cpp
render_ghost.test.cpp
GhostRenderLogic.test.cpp
positioning_ui.test.cpp
collision_mouse.test.cpp
collision_grimoire_machina.test.cpp

# pixel_render.test.cpp

# descriptors tests
descriptors/descriptors_node_descriptors.test.cpp
descriptors/descriptors_chain_descriptors.test.cpp
descriptors/AnchorBranchWorkflow.test.cpp
descriptors/ChainDescriptorBuilder.test.cpp
descriptors/depth_first_search.test.cpp
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
  part_graphs


)

catch_discover_tests(test_logic)
