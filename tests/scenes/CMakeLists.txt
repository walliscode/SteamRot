add_executable(test_scenes
SceneFactory.test.cpp
SceneManager.test.cpp
scene_helpers.cpp
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
  systems
  entity_helpers
  asset_helpers
  test_context
)

catch_discover_tests(test_scenes)
