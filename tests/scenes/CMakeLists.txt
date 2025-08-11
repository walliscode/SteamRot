add_executable(test_scenes
SceneFactory.test.cpp
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
)

catch_discover_tests(test_scenes)
