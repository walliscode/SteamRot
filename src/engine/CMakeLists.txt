
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
display
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
data_providers
)
