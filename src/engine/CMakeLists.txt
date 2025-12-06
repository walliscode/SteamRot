
add_library(engine
    Engine.cpp
    GameEngine.cpp
    engine_configuration.cpp
)




target_include_directories(engine
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)



target_link_libraries(engine PUBLIC
SFML::Graphics
scenes
display
magic_enum
entity
events
config
logger
stduuid
context
logic
flatbuffers
flatbuffers_headers
assets
configuration
)
