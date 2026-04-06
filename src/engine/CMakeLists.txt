
add_library(engine
    Engine.cpp
    GameEngine.cpp
    engine_configuration.cpp
    SandboxEngine.cpp
)


target_include_directories(engine
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(engine
PUBLIC
types
resources
SFML::Graphics
display
scenes
entity
logger
stduuid
context
data_providers
)
