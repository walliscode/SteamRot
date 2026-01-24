# Include test data compilation
include(compile_test_data.cmake)

add_executable(test_scenes
  TestScene.cpp
  SceneFactory.test.cpp
  SceneManager.test.cpp
)

target_link_libraries(test_scenes
  PRIVATE
    scenes
    entity
    context_test_helpers
    Catch2::Catch2WithMain
)

# Ensure test data is compiled before building tests
add_dependencies(test_scenes compile_scene_unit_test_data)

catch_discover_tests(test_scenes)
