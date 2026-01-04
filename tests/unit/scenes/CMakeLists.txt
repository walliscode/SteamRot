add_executable(test_scenes
  TestScene.cpp
  FlatbuffersSceneConfigurator.test.cpp
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


catch_discover_tests(test_scenes)
