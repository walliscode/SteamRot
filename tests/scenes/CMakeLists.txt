add_executable(test_scenes
SceneFactory.test.cpp
SceneManager.test.cpp
scene_helpers.cpp
TitleScene.test.cpp
)

target_compile_definitions(test_scenes PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)
target_include_directories(test_scenes
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_scenes
  PRIVATE
  Catch2::Catch2WithMain
  SFML::Graphics
  scenes
  logic
  systems
  entity_helpers
  asset_helpers
  test_context
  user_interface
  test_logic_helpers
)

catch_discover_tests(test_scenes)
