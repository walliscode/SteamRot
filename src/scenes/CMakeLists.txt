
add_library(scenes
    SceneManager.cpp
    SceneFactory.cpp
    Scene.cpp
    CraftingScene.cpp
    TitleScene.cpp
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
display
magic_enum
entity
config

logger
stduuid
context
logic
flatbuffers
flatbuffers_headers
)
