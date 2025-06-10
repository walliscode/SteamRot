
add_library(systems
    GameEngine.cpp
    SceneManager.cpp
    SceneFactory.cpp
    AssetManager.cpp
    Scene.cpp
    ActionManager.cpp
    MenuScene.cpp
    TitleScene.cpp
    EventHandler.cpp
)

# check if set definitions exist
set(definition_list
RESOURCES_DIR
DATA_OUT_DIR
)


target_include_directories(systems
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/utils
  ${CMAKE_SOURCE_DIR}/include/constants
  ${CMAKE_SOURCE_DIR}/include/enums
)

# check if targets exist
set(target_list
display
magic_enum
entity)
foreach(target_name IN LISTS target_list)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target ${target_name} does not exist")
  endif()
endforeach()


target_link_libraries(systems PUBLIC
SFML::Graphics
nlohmann_json::nlohmann_json
display
magic_enum
entity
steamrot_directory_paths
utilities
logger
stduuid
logic)
