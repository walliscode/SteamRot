
add_library(engine
    Engine.cpp
    GameEngine.cpp
    engine_configuration.cpp
)


target_include_directories(engine
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(engine
PUBLIC
resources
PRIVATE
types
interfaces
SFML::Graphics
display
scenes
components
entity
events
logger
stduuid
context
logic
assets
data_providers
)
