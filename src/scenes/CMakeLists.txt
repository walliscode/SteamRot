
add_library(scenes
    SceneManager.cpp
    SceneFactory.cpp
    Scene.cpp
    CraftingScene.cpp
    TitleScene.cpp
    ISceneConfigurator.cpp
    FlatbuffersSceneConfigurator.cpp
)

target_compile_definitions(scenes PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(scenes
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(scenes PUBLIC
SFML::Graphics
entity
context
logic
types
data_providers
interfaces
)
