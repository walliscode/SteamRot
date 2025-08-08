
add_library(systems
    GameEngine.cpp
    AssetManager.cpp
    ActionManager.cpp
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



target_link_libraries(systems PUBLIC
SFML::Graphics
scenes
display
magic_enum
entity
config
utilities
logger
stduuid
context
logic
flatbuffers
flatbuffers_headers
)
