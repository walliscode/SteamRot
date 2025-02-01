
add_library(systems
    GameEngine.cpp
    # Scene.cpp
    # SceneManager.cpp
    # SceneMainMenu.cpp
    # AssetManager.cpp
    # Action.cpp
    # DataLogger.cpp

)

# check if set definitions exist
set(definition_list
RESOURCES_DIR
DATA_OUT_DIR
)

# foreach(definition_name IN LISTS definition_list)
#   if(NOT DEFINED ${definition_name})
#     message(FATAL_ERROR "Definition ${definition_name} does not exist")
#   endif()
# endforeach()
#

target_include_directories(systems
  PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/utils)

# check if targets exist
set(target_list display)
foreach(target_name IN LISTS target_list)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target ${target_name} does not exist")
  endif()
endforeach()


target_link_libraries(systems PUBLIC
SFML::Graphics
nlohmann_json::nlohmann_json
display
)
